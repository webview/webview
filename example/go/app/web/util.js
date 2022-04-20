const width = 500
const height = 500 
const c = document.querySelector('canvas')
c.width = width
c.height = height
const gl = c.getContext('webgl')

function createShader(type, source) {
  var shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  var success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
  if (success) {
    return shader;
  }

  console.log(gl.getShaderInfoLog(shader));  // eslint-disable-line
  gl.deleteShader(shader);
  return undefined;
}

function createProgram(vertexShader, fragmentShader) {
    let vs = createShader(gl.VERTEX_SHADER, vertexShader)
    let fs = createShader(gl.FRAGMENT_SHADER, fragmentShader)

    var program = gl.createProgram();
    gl.attachShader(program, vs);
    gl.attachShader(program, fs);
    gl.linkProgram(program);
    var success = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (success) {
        return program;
    }

    console.log(gl.getProgramInfoLog(program));  // eslint-disable-line
    gl.deleteProgram(program);
    return undefined;
}

export {
    width,
    height,
    gl,
    createProgram
}