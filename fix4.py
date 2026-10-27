p = 'D:/CNode/src/tab/windows/scene_editor/scene_renderer.cpp'
t = open(p).read()
old = 'glUseProgram(cubeShaderProgram);\n\n        const int cubeMatrixLocation = glGetUniformLocation(\n            cubeShaderProgram,\n            "uViewProjection"\n        );\n\n        glUniformMatrix4fv(\n            cubeMatrixLocation,\n            1,\n            GL_FALSE,\n            viewProjection\n        );\n\n        glUseProgram(cubeShaderProgram);\n\n        const int modelLocation = glGetUniformLocation('
new = 'glUseProgram(cubeShaderProgram);\n\n        const int modelLocation = glGetUniformLocation('
assert old in t, 'old not found'
t = t.replace(old, new, 1)
open(p, 'w').write(t)
print('done')