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

            // Jangan menghubungkan window-window default secara otomatis.
            // Biarkan semua window mengambang terpisah tanpa docking/tab terkait.
            // Hanya buat dockspace kosong; window akan ditampilkan terpisah.

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
