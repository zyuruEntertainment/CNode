#pragma once

namespace CNode
{
    // Per-window-instance state for the Object List window.
    // Keyed by window title so multiple object list windows do not
    // share rename / hover state (mirrors Scene Editor state handling).
    struct ObjectListState
    {
        bool renaming = false;
        unsigned long long renamingId = 0;
        char renameBuffer[256] = {};
        bool itemHovered = false;
    };
}
