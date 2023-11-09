#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "olc-net.h"

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

class CustomClient : public olc::net::client_interface<CustomMsgTypes> {
public:
<<<<<<< HEAD
	void PingServer()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution this
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;
		Send(msg);
	}
=======
    void PingServer() {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerPing;
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
        msg << timeNow;
        Send(msg);
    }
>>>>>>> 55eb5680ae4ee81a5626d1e387812ee3bb6c366d
};

int main() {
    CustomClient c;
    c.Connect("127.0.0.1", 60000);

    Display* dpy = XOpenDisplay(nullptr);
    if (dpy == nullptr) {
        std::cerr << "Error: Could not open display." << std::endl;
        return 1;
    }

    XSelectInput(dpy, DefaultRootWindow(dpy), KeyPressMask | KeyReleaseMask);
    bool key[3] = {false, false, false};
    bool old_key[3] = {false, false, false};
    bool bQuit = false;

    while (!bQuit) {
        XEvent event;
        XNextEvent(dpy, &event);

        switch (event.type) {
            case KeyPress: {
                KeySym keySym = XLookupKeysym(&event.xkey, 0);
                if (keySym == XK_1) key[0] = true;
                else if (keySym == XK_2) key[1] = true;
                else if (keySym == XK_3) key[2] = true;
                break;
            }
            case KeyRelease: {
                KeySym keySym = XLookupKeysym(&event.xkey, 0);
                if (keySym == XK_1) key[0] = false;
                else if (keySym == XK_2) key[1] = false;
                else if (keySym == XK_3) key[2] = false;
                break;
            }
        }

        if (key[0] && !old_key[0]) {
            std::cout << "1 key pressed" << std::endl;
            c.PingServer(); // Ping the server when key 1 is pressed
        }

        if (key[1] && !old_key[1]) std::cout << "2 key pressed" << std::endl;
        if (key[2] && !old_key[2]) bQuit = true;

<<<<<<< HEAD
				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request
					std::cout << "Server Accepted Connection\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				break;

				case CustomMsgTypes::ServerPing:
				{
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
					break;
				}
				break;
				}
			}
			else
			{
				std::cout << "Server Down\n";
				bQuit = true;
			}
		}
		return 0;
	}
=======
        for (int i = 0; i < 3; i++) old_key[i] = key[i];

        if (c.IsConnected()) {
            if (!c.Incoming().empty()) {
                auto msg = c.Incoming().pop_front().msg;
                switch (msg.header.id) {
                    case CustomMsgTypes::ServerPing: {
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                        break;
                    }
                }
            }
        } else {
            std::cout << "Server Down\n";
            bQuit = true;
        }
    }

    XCloseDisplay(dpy);
    return 0;
>>>>>>> 55eb5680ae4ee81a5626d1e387812ee3bb6c366d
}