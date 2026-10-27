p='D:/CNode/src/tab/windows/scene_editor/scene_renderer.cpp'
t=open(p).read()
start=t.find('const Scene& scene = GetScene();')
end=t.find('glBindVertexArray(0);', start)
assert start!=-1 and end!=-1
t=t[:start] + t[end:]
open(p,'w').write(t)
print("done")