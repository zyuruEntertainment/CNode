p='D:/CNode/src/tab/windows/scene_editor/scene_renderer.cpp'
t=open(p).read()
start=t.find('glUseProgram(cubeShaderProgram);')
end=t.find('glBindVertexArray(0);\n        glUseProgram(0);', start)+len('glBindVertexArray(0);\n        glUseProgram(0);')
assert start!=-1 and end!=-1
t=t[:start]+t[end:]
open(p,'w').write(t)
print('done')