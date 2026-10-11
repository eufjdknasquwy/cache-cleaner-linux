#pragma once
#include "core/cleaner.h"
#include <gtkmm.h>
namespace ui
{
    class MessageDialogHelper
    {
        public:
            MessageDialogHelper() = delete;
            static void spawn_result_dialog(core::CacheCleaner::ClearReport &del_result);

        private:
    };
} // namespace ui
