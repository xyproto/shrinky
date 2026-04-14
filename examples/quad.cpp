/// \file
/// One-quad example.

//######################################
// Include #############################
//######################################

#include "shrinky.h"

//######################################
// Define ##############################
//######################################

/// Screen mode.
///
/// Negative values windowed.
/// Positive values fullscreen.
#define DISPLAY_MODE -720

/// \cond
#if (0 > (DISPLAY_MODE))
#define SCREEN_F 0
#define SCREEN_H (-(DISPLAY_MODE))
#elif (0 < (DISPLAY_MODE))
#define SCREEN_F 1
#define SCREEN_H (DISPLAY_MODE)
#else
#error "invalid display mode (pre)"
#endif
#if ((800 == SCREEN_H) || (1200 == SCREEN_H))
#define SCREEN_W ((SCREEN_H / 10) * 16)
#else
#define SCREEN_W (((SCREEN_H * 16) / 9) - (((SCREEN_H * 16) / 9) % 4))
#endif
/// \endcond

/// Fullscreen on/off.
#define FLAG_FULLSCREEN 0

/// Intro length (in milliseconds)
#define INTRO_LENGTH (16000)

//######################################
// Global functions ####################
//######################################

/// Global SDL window storage.
static SDL_Window *g_sdl_window;

#if defined(SHRINKY_GLESV2) && defined(SHRINKY_VIDEOCORE)
#include "shrinky_egl.h"
#include "shrinky_videocore.h"
#endif

/// Swap buffers.
///
/// Uses global data.
static void swap_buffers()
{
#if defined(SHRINKY_GLESV2) && defined(SHRINKY_VIDEOCORE)
  shrinky_eglSwapBuffers(g_egl_display, g_egl_surface);
#else
  shrinky_SDL_GL_SwapWindow(g_sdl_window);
#endif
}

/// Tear down initialized systems.
///
/// Uses global data.
static void teardown()
{
#if defined(SHRINKY_GLESV2) && defined(SHRINKY_VIDEOCORE)
  egl_quit(g_egl_display);
  shrinky_bcm_host_deinit();
#endif
  shrinky_SDL_Quit();
}

//######################################
// Shaders #############################
//######################################

#if defined(SHRINKY_GLESV2)

/// Quad vertex shader.
static const char *g_shader_vertex_quad = ""
"attribute vec2 a;"
"varying mediump vec2 b;"
"precision mediump float;"
"void main()"
"{"
"b=a;"
"gl_Position=vec4(a,0.,1.);"
"}";

/// Quad fragment shader.
static const char *g_shader_fragment_quad = ""
"varying mediump vec2 b;"
"uniform highp float t;"
"precision mediump float;"
"void main()"
"{"
"gl_FragColor=vec4(b.x,sin(t/777.)*.5+.5,b.y,1.);"
"}";

/// Uniform location.
static GLint g_uniform_t;

#else

/// Quad vertex shader.
static const char *g_shader_vertex_quad = ""
"#version 430\n"
"in vec2 a;"
"out vec2 b;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"};"
"void main()"
"{"
"b=a;"
"gl_Position=vec4(a,0,1);"
"}";

/// Quad fragment shader.
static const char *g_shader_fragment_quad = ""
"#version 430\n"
"layout(location=0)uniform float t;"
"in vec2 b;"
"out vec4 o;"
"void main()"
"{"
"o=vec4(b.x,sin(t/777.)*.5+.5,b.y,1.);"
"}";

#endif

/// \cond
static GLuint g_program_fragment;
/// \endcond

#if defined(SHRINKY_GLESV2)

/// \brief Create shader.
///
/// \param source Source of the shader.
/// \return Shader ID.
GLuint create_shader(const char *source, GLenum type)
{
  GLuint ret = shrinky_glCreateShader(type);

  shrinky_glShaderSource(ret, 1, &source, NULL);
  shrinky_glCompileShader(ret);

  return ret;
}

/// \brief Create program.
///
/// \param vertex Vertex shader.
/// \param fragment Fragment shader.
/// \return Program ID
GLuint create_program(const char *vertex, const char *fragment)
{
  GLuint ret = shrinky_glCreateProgram();

  shrinky_glAttachShader(ret, create_shader(vertex, GL_VERTEX_SHADER));
  shrinky_glAttachShader(ret, create_shader(fragment, GL_FRAGMENT_SHADER));

  shrinky_glLinkProgram(ret);

  return ret;
}

#endif

//######################################
// Draw ################################
//######################################

/// \brief Draw the world.
///
/// \param ticks Milliseconds.
static void draw(unsigned ticks)
{
#if defined(SHRINKY_GLESV2)
  shrinky_glUniform1f(g_uniform_t, ticks);
  {
    int8_t array[] =
    {
      -3, 1,
      1, -3,
      1, 1,
    };

    shrinky_glVertexAttribPointer(0, 2, GL_BYTE, GL_FALSE, 0, array);
    shrinky_glEnableVertexAttribArray(0);
    shrinky_glDrawArrays(GL_TRIANGLES, 0, 3);
  }
#else
  shrinky_glProgramUniform1f(g_program_fragment, 0, ticks);

  shrinky_glRects(-1, -1, 1, 1);
#endif
}

//######################################
// Main ################################
//######################################

#if defined(USE_LD)
int main()
#else
void _start()
#endif
{
  shrinky();
  shrinky_SDL_Init(SDL_INIT_VIDEO);
#if defined(SHRINKY_GLESV2) && !defined(SHRINKY_VIDEOCORE)
  shrinky_SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  shrinky_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  shrinky_SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
  g_sdl_window = shrinky_SDL_CreateWindow(NULL, SCREEN_W,
      SCREEN_H, SDL_WINDOW_OPENGL | (FLAG_FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0));
#if defined(SHRINKY_GLESV2) && defined(SHRINKY_VIDEOCORE)
  videocore_create_native_window(SCREEN_W, SCREEN_H);
  egl_init(reinterpret_cast<NativeWindowType>(&g_egl_native_window), &g_egl_display, &g_egl_surface);
#else
  shrinky_SDL_GL_CreateContext(g_sdl_window);
#endif
  shrinky_SDL_HideCursor();

#if defined(USE_LD)
  glewInit();
#endif

#if defined(SHRINKY_GLESV2)
  g_program_fragment = create_program(g_shader_vertex_quad, g_shader_fragment_quad);
  shrinky_glUseProgram(g_program_fragment);
  g_uniform_t = shrinky_glGetUniformLocation(g_program_fragment, "t");
#else
  // Shader generation inline.
  GLuint pipeline;
  GLuint program_vert = shrinky_glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &g_shader_vertex_quad);
  g_program_fragment = shrinky_glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &g_shader_fragment_quad);

  shrinky_glGenProgramPipelines(1, &pipeline);
  shrinky_glBindProgramPipeline(pipeline);
  shrinky_glUseProgramStages(pipeline, 1, program_vert);
  shrinky_glUseProgramStages(pipeline, 2, g_program_fragment);
#endif

  unsigned start_ticks = shrinky_SDL_GetTicks();

  for(;;)
  {
    SDL_Event event;
    unsigned curr_ticks = shrinky_SDL_GetTicks() - start_ticks;

    shrinky_SDL_PollEvent(&event);

    if((curr_ticks >= INTRO_LENGTH) || (event.type == SDL_EVENT_KEY_DOWN))
    {
      break;
    }

    draw(curr_ticks);
    swap_buffers();
  }

  teardown();
#if defined(USE_LD)
  return 0;
#else
  asm_exit();
#endif
}

//######################################
// End #################################
//######################################

