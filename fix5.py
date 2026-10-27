p='D:/CNode/src/tab/windows/scene_editor/scene_renderer.cpp'
t=open(p).read()
start=t.find('        for (const SceneObject& object : scene.objects)')
end=t.find('        glBindVertexArray(0);\n        glUseProgram(0);', start)
assert start!=-1 and end!=-1, (start,end)
newblock='''        for (const SceneObject& object : scene.objects)
        {
            (void)object;
            continue;
        }

'''
t=t[:start]+newblock+t[end:]
open(p,'w').write(t)
print('done')