from dnload.glsl_block import tokenize
from dnload.glsl_block_function import glsl_parse_function
from dnload.glsl_block_default import glsl_parse_default
from dnload.glsl_block_inout import glsl_parse_inout
from dnload.glsl_block_pervertex import glsl_parse_pervertex
from dnload.glsl_block_preprocessor import glsl_parse_preprocessor
from dnload.glsl_block_uniform import glsl_parse_uniform

########################################
# Functions ############################
########################################
 
def glsl_parse(source):
  """Parse given source."""
  ret = []
  content = []
  # First, extract preprocessor directives.
  for ii in source.splitlines():
    line = ii.strip()
    if ii.startswith("#"):
      ret += [glsl_parse_preprocessor(line)]
    else:
      content += line
  content = tokenize("".join(content))
  return ret + glsl_parse_tokenized(content)

def glsl_parse_tokenized(source):
  """Parse tokenized source."""
  # End of input.
  if not source:
    return []
  # Advance content, try different parses one at a time.
  (block, remaining) = glsl_parse_inout(source)
  if block:
    return [block] + glsl_parse_tokenized(remaining)
  (block, remaining)  = glsl_parse_pervertex(source)
  if block:
    return [block] + glsl_parse_tokenized(remaining)
  (block, remaining) = glsl_parse_uniform(source)
  if block:
    return [block] + glsl_parse_tokenized(remaining)
  (block, remaining) = glsl_parse_function(source)
  if block:
    return [block] + glsl_parse_tokenized(remaining)
  # Fallback, should never happen.
  return glsl_parse_default(source)