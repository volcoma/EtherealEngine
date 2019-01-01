////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <X11/Xatom.h>
#include <mml/system/err.hpp>
#include <mml/window/unix/clipboard_impl.hpp>
#include <mml/window/unix/display.hpp>
#include <vector>
#include <chrono>

namespace
{
// Filter the events received by windows (only allow those matching a specific window)
Bool checkEvent(::Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<::Window>(userData);
}
} // namespace

namespace mml
{
namespace priv
{

////////////////////////////////////////////////////////////
std::string clipboard_impl::get_string()
{
	return get_instance().get_string_impl();
}

////////////////////////////////////////////////////////////
void clipboard_impl::set_string(const std::string& text)
{
	get_instance().set_string_impl(text);
}

////////////////////////////////////////////////////////////
void clipboard_impl::process_events()
{
	get_instance().process_events_impl();
}

////////////////////////////////////////////////////////////
clipboard_impl::clipboard_impl()
	: m_window(0)
	, m_requestResponded(false)
{
	// Open a connection with the X server
	m_display = open_display();

	// Get the atoms we need to make use of the clipboard
	m_clipboard = get_atom("CLIPBOARD", false);
	m_targets = get_atom("TARGETS", false);
	m_text = get_atom("TEXT", false);
	m_utf8String = get_atom("UTF8_STRING", true);
	m_targetProperty = get_atom("SFML_CLIPBOARD_TARGET_PROPERTY", false);

	// Create a hidden window that will broker our clipboard interactions with X
	m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, 1, 1, 0, 0, 0);

	// Register the events we are interested in
	XSelectInput(m_display, m_window, SelectionNotify | SelectionClear | SelectionRequest);
}

////////////////////////////////////////////////////////////
clipboard_impl::~clipboard_impl()
{
	// Destroy the window
	if(m_window)
	{
		XDestroyWindow(m_display, m_window);
		XFlush(m_display);
	}

	// Close the connection with the X server
	close_display(m_display);
}

////////////////////////////////////////////////////////////
clipboard_impl& clipboard_impl::get_instance()
{
	static clipboard_impl instance;

	return instance;
}

////////////////////////////////////////////////////////////
std::string clipboard_impl::get_string_impl()
{
	// Check if anybody owns the current selection
	if(XGetSelectionOwner(m_display, m_clipboard) == None)
	{
		m_clipboardContents.clear();

		return m_clipboardContents;
	}

	// Process any already pending events
	process_events();

	m_requestResponded = false;

	// Request the current selection to be converted to UTF-8 (or STRING
	// if UTF-8 is not available) and written to our window property
	XConvertSelection(m_display, m_clipboard, (m_utf8String != None) ? m_utf8String : XA_STRING,
					  m_targetProperty, m_window, CurrentTime);


	auto last = std::chrono::steady_clock::now();
	// Wait for a response for up to 1000ms
	while(!m_requestResponded && (std::chrono::duration_cast<std::chrono::milliseconds>(now - last) <
								  std::chrono::milliseconds(1000)))
		process_events();

	// If no response was received within the time period, clear our clipboard contents
	if(!m_requestResponded)
		m_clipboardContents.clear();

	return m_clipboardContents;
}

////////////////////////////////////////////////////////////
void clipboard_impl::set_string_impl(const std::string& text)
{
	m_clipboardContents = text;

	// Set our window as the current owner of the selection
	XSetSelectionOwner(m_display, m_clipboard, m_window, CurrentTime);

	// Check if setting the selection owner was successful
	if(XGetSelectionOwner(m_display, m_clipboard) != m_window)
		err() << "Cannot set clipboard string: Unable to get ownership of X selection" << std::endl;
}

////////////////////////////////////////////////////////////
void clipboard_impl::process_events_impl()
{
	XEvent event;

	// Pick out the events that are interesting for this window
	while(XCheckIfEvent(m_display, &event, &checkEvent, reinterpret_cast<XPointer>(m_window)))
		m_events.push_back(event);

	// Handle the events for this window that we just picked out
	while(!m_events.empty())
	{
		event = m_events.front();
		m_events.pop_front();
		process_event(event);
	}
}

////////////////////////////////////////////////////////////
void clipboard_impl::process_event(XEvent& windowEvent)
{
	switch(windowEvent.type)
	{
		case SelectionClear:
		{
			// We don't have any resources we need to clean up
			// when losing selection ownership so we don't do
			// anything when we receive SelectionClear
			// We will still respond to any future SelectionRequest
			// events since doing so doesn't really do any harm
			break;
		}
		case SelectionNotify:
		{
			// Notification that the current selection owner
			// has responded to our request

			XSelectionEvent& selectionEvent = *reinterpret_cast<XSelectionEvent*>(&windowEvent.xselection);

			m_clipboardContents.clear();

			// If retrieving the selection fails or conversion is unsuccessful
			// we leave the contents of the clipboard empty since we don't
			// own it and we don't know what it could currently be
			if((selectionEvent.property == None) || (selectionEvent.selection != m_clipboard))
				break;

			Atom type;
			int format;
			unsigned long items;
			unsigned long remainingBytes;
			unsigned char* data = 0;

			// The selection owner should have wrote the selection
			// data to the specified window property
			int result = XGetWindowProperty(m_display, m_window, m_targetProperty, 0, 0x7fffffff, False,
											AnyPropertyType, &type, &format, &items, &remainingBytes, &data);

			if(result == Success)
			{
				// We don't support INCR for now
				// It is very unlikely that this will be returned
				// for purely text data transfer anyway
				if(type != get_atom("INCR", false))
				{
					// Only copy the data if the format is what we expect
					if((type == m_utf8String) && (format == 8))
					{
						m_clipboardContents = std::string(data, data + items);
					}
					else if((type == XA_STRING) && (format == 8))
					{
						m_clipboardContents = std::string(data, data + items);
					}
				}

				XFree(data);

				// The selection requestor must always delete the property themselves
				XDeleteProperty(m_display, m_window, m_targetProperty);
			}

			m_requestResponded = true;

			break;
		}
		case SelectionRequest:
		{
			// Respond to a request for our clipboard contents
			XSelectionRequestEvent& selectionRequestEvent =
				*reinterpret_cast<XSelectionRequestEvent*>(&windowEvent.xselectionrequest);

			// Our reply
			XSelectionEvent selectionEvent;

			selectionEvent.type = SelectionNotify;
			selectionEvent.requestor = selectionRequestEvent.requestor;
			selectionEvent.selection = selectionRequestEvent.selection;
			selectionEvent.property = selectionRequestEvent.property;
			selectionEvent.time = selectionRequestEvent.time;

			if(selectionRequestEvent.selection == m_clipboard)
			{
				if(selectionRequestEvent.target == m_targets)
				{
					// Respond to a request for our valid conversion targets
					std::vector<Atom> targets;

					targets.push_back(m_targets);
					targets.push_back(m_text);
					targets.push_back(XA_STRING);

					if(m_utf8String != None)
						targets.push_back(m_utf8String);

					XChangeProperty(m_display, selectionRequestEvent.requestor,
									selectionRequestEvent.property, XA_ATOM, 32, PropModeReplace,
									reinterpret_cast<unsigned char*>(&targets[0]), targets.size());

					// Notify the requestor that they can read the targets from their window property
					selectionEvent.target = m_targets;

					XSendEvent(m_display, selectionRequestEvent.requestor, True, NoEventMask,
							   reinterpret_cast<XEvent*>(&selectionEvent));

					break;
				}
				else if((selectionRequestEvent.target == XA_STRING) ||
						((m_utf8String == None) && (selectionRequestEvent.target == m_text)))
				{
					// Respond to a request for conversion to a Latin-1 string
					std::string data = m_clipboardContents;

					XChangeProperty(m_display, selectionRequestEvent.requestor,
									selectionRequestEvent.property, XA_STRING, 8, PropModeReplace,
									reinterpret_cast<const unsigned char*>(data.c_str()), data.size());

					// Notify the requestor that they can read the data from their window property
					selectionEvent.target = XA_STRING;

					XSendEvent(m_display, selectionRequestEvent.requestor, True, NoEventMask,
							   reinterpret_cast<XEvent*>(&selectionEvent));

					break;
				}
				else if((m_utf8String != None) && ((selectionRequestEvent.target == m_utf8String) ||
												   (selectionRequestEvent.target == m_text)))
				{
					// Respond to a request for conversion to a UTF-8 string
					// or an encoding of our choosing (we always choose UTF-8)
					// std::basic_string<uint8> data = m_clipboardContents.toUtf8();
					std::string data = m_clipboardContents;

					XChangeProperty(m_display, selectionRequestEvent.requestor,
									selectionRequestEvent.property, m_utf8String, 8, PropModeReplace,
									reinterpret_cast<const unsigned char*>(data.c_str()), data.size());

					// Notify the requestor that they can read the data from their window property
					selectionEvent.target = m_utf8String;

					XSendEvent(m_display, selectionRequestEvent.requestor, True, NoEventMask,
							   reinterpret_cast<XEvent*>(&selectionEvent));

					break;
				}
			}

			// Notify the requestor that we could not respond to their request
			selectionEvent.target = selectionRequestEvent.target;
			selectionEvent.property = None;

			XSendEvent(m_display, selectionRequestEvent.requestor, True, NoEventMask,
					   reinterpret_cast<XEvent*>(&selectionEvent));

			break;
		}
		default:
			break;
	}
}

} // namespace priv

} // namespace mml
