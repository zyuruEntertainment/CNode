p='D:/CNode/src/tab/windows/scene_editor/scene_renderer.cpp'
t=open(p).read()
old = '''glUseProgram(cubeShaderProgram);

        const int cubeMatrixLocation = glGetUniformLocation(
            cubeShaderProgram,
            "uViewProjection"
        );

        glUniformMatrix4fv(
            cubeMatrixLocation,
            1,
            GL_FALSE,
            viewProjection
        );

        glUseProgram(cubeShaderProgram);

        const int modelLocation = glGetUniformLocation('''
new = '''glUseProgram(cubeShaderProgram);

        const int modelLocation = glGetUniformLocation('''
assert old in t, "old not found"
t = t.replace(old, new, 1)
open(p, 'w').write(t)
print("done")'''
open('D:/CNode/fix3.py','w').write(t) if False else None
EOF