import { width, height, gl, createProgram } from "./util.js"
    
const vs = `
attribute vec4 pos;

void main() {
    gl_Position = pos;
}	
`

const fs = `
precision mediump float;	

void main() {
    gl_FragColor = vec4(1, 0, 0, 1);
}
`

const program = createProgram(vs, fs)
gl.useProgram(program)

gl.viewport(0,0, width, height)

gl.clearColor(0,0,0,1)
gl.clear(gl.COLOR_BUFFER_BIT)

const pos_buffer = gl.createBuffer()
gl.bindBuffer(gl.ARRAY_BUFFER, pos_buffer)

var points = [
    -0.5, 0,
    0, 0.5,
    0.5, 0
]
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(points), gl.STATIC_DRAW)

const pos_attribute_location = gl.getAttribLocation(program, 'pos')

gl.enableVertexAttribArray(pos_attribute_location)

gl.vertexAttribPointer(pos_attribute_location, 2, gl.FLOAT, false, 0, 0 )

gl.drawArrays(gl.TRIANGLES, 0, 3)
