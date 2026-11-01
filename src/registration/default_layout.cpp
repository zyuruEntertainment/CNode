#include "registration/default_layout.h"
#include "tab/windows/window_instance_manager.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <filesystem>
#include <fstream>

namespace CNode
{
    namespace
    {
        constexpr const char* kLayoutMarker =
            ".cnode_default_layout_v1";

        WindowInstance* FindWindow(const char* typeID)
        {
            auto& instances = GetWindowInstances();

            for (WindowInstance& instance : instances)
            {
                if (instance.typeID == typeID)
                    return &instance;
            }

            return nullptr;
        }

        void EnsureDefaultWindows()
        {
            constexpr const char* defaultWindows[] =
            {
                "scene_editor",
                "object_list",
                "node_graph",
                "inspector"
            };

            for (const char* typeID : defaultWindows)
            {
                if (!FindWindow(typeID))
                    AddWindow(typeID);
            }
        }

        void DockWindow(
            const char* typeID,
            ImGuiID dockID)
        {
            WindowInstance* instance = FindWindow(typeID);

            if (!instance)
                return;

            ImGui::DockBuilderDockWindow(
                instance->title.c_str(),
                dockID
            );
        }


        void BuildDefaultLayout(ImGuiID dockspaceID)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::DockBuilderRemoveNode(dockspaceID);

            ImGui::DockBuilderAddNode(
                dockspaceID,
                ImGuiDockNodeFlags_DockSpace
            );

            ImGui::DockBuilderSetNodeSize(
                dockspaceID,
                viewport->WorkSize
            );

            // Pecah dockspace utama menjadi kolom kiri (~60% lebar)
            // dan kolom kanan (~40% lebar).
            ImGuiID dockID_left, dockID_right;
            ImGui::DockBuilderSplitNode(
                dockspaceID,
                ImGuiDir_Right,
                0.40f,
                &dockID_right,
                &dockID_left
            );

            // Pecah kolom kiri secara vertikal: Scene Editor di atas (~65% tinggi),
            // Node Graph di bawah (~35% tinggi).
            ImGuiID dockID_scene_editor, dockID_node_graph;
            ImGui::DockBuilderSplitNode(
                dockID_left,
                ImGuiDir_Down,
                0.35f,
                &dockID_node_graph,
                &dockID_scene_editor
            );

            // Pecah kolom kanan secara horizontal: Object List di kiri (~50% lebar kolom),
            // Inspector di kanan (~50% lebar kolom). Keduanya berdampingan, bukan dalam satu tab.
            ImGuiID dockID_object_list, dockID_inspector;
            ImGui::DockBuilderSplitNode(
                dockID_right,
                ImGuiDir_Right,
                0.50f,
                &dockID_inspector,
                &dockID_object_list
            );

            // Pasang masing-masing window ke dock area-nya sendiri.
            DockWindow("scene_editor", dockID_scene_editor);
            DockWindow("node_graph", dockID_node_graph);
            DockWindow("object_list", dockID_object_list);
            DockWindow("inspector", dockID_inspector);

            ImGui::DockBuilderFinish(dockspaceID);
        }

    }

    void SetupDefaultDockLayout()
    {
        static bool initialized = false;

        if (initialized)
            return;

        initialized = true;

        // Registry harus sudah dijalankan sebelum fungsi ini.
        EnsureDefaultWindows();

        // Bangun layout default hanya sekali untuk versi ini.
        // Perubahan layout berikutnya disimpan oleh Dear ImGui.
        if (std::filesystem::exists(kLayoutMarker))
            return;

        const ImGuiID dockspaceID =
            ImGui::GetID("CNodeDockspace");

        BuildDefaultLayout(dockspaceID);

        std::ofstream marker(kLayoutMarker);

        if (marker)
            marker << "1\n";
    }
}
