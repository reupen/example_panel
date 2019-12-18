/**
 * \file main.cpp
 *
 * \brief Example panel component
 *
 * This component is an example of a very simple multiple instance panel that does not take any keyboard input.
 */

// Included before windows.h, because pfc.h includes winsock2.h
#include "../pfc/pfc.h"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "../foobar2000/SDK/foobar2000.h"

#include "../columns_ui-sdk/ui_extension.h"

/** Declare some information about our component */
DECLARE_COMPONENT_VERSION("Example Columns UI Panel", "0.1", "compiled: " __DATE__);

/**
 * The GUID that identifies our panel.
 *
 * You must change this – don't use the same one!
 */
constexpr GUID extension_guid = {0x79f574f1, 0xdc70, 0x4f3f, {0x81, 0x55, 0x38, 0x4b, 0x0, 0xae, 0x6, 0x79}};

/** Our window class. */
class ExampleWindow : public uie::container_ui_extension {
public:
    const GUID& get_extension_guid() const override { return extension_guid; }
    void get_name(pfc::string_base& out) const override { out = "Example"; }
    void get_category(pfc::string_base& out) const override { out = "Panels"; }
    unsigned get_type() const override { return uie::type_panel; }

private:
    class_data& get_class_data() const override
    {
        __implement_get_class_data(_T("{79F574F1-DC70-4f3f-8155-384B00AE0679}"), true);
    }

    /** Our window procedure */
    LRESULT on_message(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) override;
    void get_menu_items(uie::menu_hook_t& p_hook) override;

    /** Our child window */
    HWND wnd_static{nullptr};
};

void ExampleWindow::get_menu_items(uie::menu_hook_t& p_hook)
{
    const uie::menu_node_ptr close_panel = new uie::simple_command_menu_node(
        "Close panel", "Removes this panel from the layout.", 0, [self = service_ptr_t<ExampleWindow>(this)] {
            const HWND wnd = self->get_wnd();
            uie::window_host_ptr host = self->get_host();
            uie::window_ptr(self)->destroy_window();
            host->relinquish_ownership(wnd);
        });
    p_hook.add_node(close_panel);
}

LRESULT ExampleWindow::on_message(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        RECT rc;
        GetClientRect(wnd, &rc);

        /** Create a static window, with text "Example". */
        wnd_static = CreateWindowEx(0, WC_STATIC, _T("Example panel"), WS_CHILD | WS_VISIBLE, 0, 0, rc.right, rc.bottom,
            wnd, HMENU(0), core_api::get_my_instance(), nullptr);
    } break;
    case WM_SIZE:
        /** The static control requires this to redraw correctly. */
        RedrawWindow(wnd_static, 0, 0, RDW_INVALIDATE | RDW_ERASE);
        /** Reposition the static control. */
        SetWindowPos(wnd_static, 0, 0, 0, LOWORD(lp), HIWORD(lp), SWP_NOZORDER);
        break;
    case WM_DESTROY:
        /** DefWindowProc will destroy our child window. Set our window handle to nullptr now. */
        wnd_static = nullptr;
        break;
    }
    return DefWindowProc(wnd, msg, wp, lp);
}

static uie::window_factory<ExampleWindow> example_window_factory;
