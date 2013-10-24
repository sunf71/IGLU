#include <stdio.h>
#include <stdlib.h>

#include "iglu.h"
#include <FL/names.h>

#if defined _WIN32 || defined _WIN64
        #include <GL/wglew.h>
#else
        #include <GL/glxew.h>
#endif


using namespace iglu;

static bool g_isGlewInitialized = false;
static IGLUTexture2D::Ptr g_texPlusButton = 0;


IGLUWindow::IGLUWindow( int width, int height, const char *title ) :
	Fl_Gl_Window( 25, 50, width, height, title ), 
	m_currentWidth(width), m_currentHeight(height), 
	m_isInitialized(false), m_redrawOnIdle(true), m_fltkIdleAdded(false), m_isRunning(false), 
	m_displayFPS(false), m_overPlusButton(false), m_widgetWindow(0),
	m_fltkMode(FL_RGB | FL_DOUBLE | FL_DEPTH),
	m_callbackKeyboard(0), m_callbackIdle(0), m_preprocessOnGLInit(0), m_callbackDisplay(0),
	m_callbackReshape(0), m_callbackButton(0), m_callbackActiveMove(0), m_callbackPassiveMove(0),
	m_callbackEntry(0), m_callbackVisible(0), m_callbackSpecial(0),
	frameRate(0)
{
}

IGLUWindow::~IGLUWindow()
{
}

void IGLUWindow::draw( void )
{
	// Something has changed
	if (!valid())
	{
		// Should not happen...  But make sure we've initialized window-specific GL resources
		if (!m_isInitialized)
			Initialize();

		// Do a reshape here?  (That's the other reason we may be invalid...)
		if (m_callbackReshape)
			(*m_callbackReshape)( w(), h() );

		// We've handled anything causing invalidity...  Mark as valid
		valid(1);
	}

	// If the user asked us to time the draws, do that
	if (m_displayFPS && frameRate) frameRate->StartFrame();

	// Draw stuff the user wants
	if (m_callbackDisplay) 
		(*m_callbackDisplay)();	

	// Setup the viewport to the whole window so we can control where to draw
	//    the misc window decorations.
	glViewport( 0, 0, w(), h() );

	// If the user asked us to time the draws, do that
	if (m_displayFPS && frameRate)
	{
		char buf[64];
		sprintf( buf, "%.1f fps", frameRate->EndFrame() );
		IGLUDraw::DrawText( IGLU_FONT_FIXED, 0, 0, buf );
	}

	if (m_widgetWindow)
	{
		float xOffset = (2.0f*g_texPlusButton->GetWidth())/w();
		float yOffset = (2.0f*g_texPlusButton->GetHeight())/h();
		IGLUDraw::Texture( g_texPlusButton, 
			               1.0-xOffset, -1.0, 
						   1.0, -1+yOffset, 
						   m_overPlusButton ? IGLU_DRAW_TEX_SIMPLE_BRIGHTEN : 0 );
	}
}

void IGLUWindow::CheckButtonCoverage( int x, int y )
{
	m_overPlusButton = true;
}

// Handle some sort of event...
int IGLUWindow::handle( int event )
{
	// Vars needed in the switch.  Compilers don't like them
	//    being defined inside the switch.
	int button;
	bool over;

	switch( event ) 
	{
	case FL_SHOW:
		if (m_callbackVisible) (*m_callbackVisible)( IGLU_EVENT_VISIBLE );
		break;

	case FL_HIDE:
		if (m_callbackVisible) (*m_callbackVisible)( IGLU_EVENT_NOT_VISIBLE );
		break;

	case FL_ENTER:
		if (m_callbackEntry) { (*m_callbackEntry)( IGLU_EVENT_ENTERED ); return 1; }
		if (m_callbackPassiveMove) return 1;
		return 1;  // We always want FL_MOVE, so we need to return 1

	case FL_LEAVE:
		if (m_callbackEntry) { (*m_callbackEntry)( IGLU_EVENT_LEFT ); return 1; }
		if (m_callbackPassiveMove) return 1;
		return 1;  // We always want FL_MOVE, so we need to return 1

	case FL_KEYBOARD:
		// Check for 'Esc' --> Hard code to 'quit'
		if (Fl::event_text()[0] == 27)
			exit(0);

		if (m_callbackKeyboard && Fl::event_text()[0])
		{
			(*m_callbackKeyboard)( Fl::event_text()[0], Fl::event_x(), Fl::event_y() );
			return 1;
		}
		else if (m_callbackSpecial && !Fl::event_text()[0])
		{
			int key = Fl::event_key();
			key = key - ( (key > FL_F && key <= FL_F_Last) ? FL_F : 0 );
			(*m_callbackSpecial)( key, Fl::event_x(), Fl::event_y() );
			return 1;
		}
		break;

	case FL_PUSH:
		if (m_overPlusButton && m_widgetWindow) { m_widgetWindow->show(); return 1; }
	case FL_RELEASE:
		if (!m_callbackButton)
			return Fl_Gl_Window::handle( event );
		button = Fl::event_button();
		(*m_callbackButton)( ((button==FL_LEFT_MOUSE) ? IGLU_EVENT_LEFT_BUTTON : 
			                                           (button==FL_RIGHT_MOUSE ? IGLU_EVENT_RIGHT_BUTTON : 
													                             IGLU_EVENT_MIDDLE_BUTTON )), 
			                 ((event==FL_PUSH) ? IGLU_EVENT_DOWN : IGLU_EVENT_UP), 
			                 Fl::event_x(), Fl::event_y() );
		return 1;
	case FL_DRAG:
		if (m_callbackActiveMove) { (*m_callbackActiveMove)( Fl::event_x(), Fl::event_y() ); return 1; }
		break;

	case FL_MOVE:
		if (m_widgetWindow) 
		{
			over = Fl::event_inside( w()-32, h()-32, 32, 32 ) > 0;
			if (over) CheckButtonCoverage( Fl::event_x(), Fl::event_y() );
			m_overPlusButton = (!over ? false : m_overPlusButton);
		}
		if (m_callbackPassiveMove) { (*m_callbackPassiveMove)( Fl::event_x(), Fl::event_y() ); return 1; }
		break;
	}

	return Fl_Gl_Window::handle( event );
}

void IGLUWindow::Initialize( void )
{
	// Do any preprocessing required prior to drawing
	if (m_preprocessOnGLInit)
		(*m_preprocessOnGLInit)();

	// We're done initializing
	m_isInitialized = true;
}

void IGLUWindow::CreateWindow( int argc, char **argv )
{
	show( argc, argv );  // Create an FLTK window
	valid(0);            // Mark the window as invalid
	context_valid(0);    // Mark our GL context as invalid
	make_current();      // Grab a GL context for this window

	// Initialize GLEW (if we haven't yet)
	if (!g_isGlewInitialized)
	{
		glewInit();
		g_isGlewInitialized = true;
	}

	// Load the textures we'll use for our UI buttons, if they aren't around...
	if (!g_texPlusButton)
		g_texPlusButton = new IGLUTexture2D( IGLU_BUILTIN_TEX_PLUSBUTTON, 
		                                     IGLU_MIN_LINEAR | IGLU_MAG_LINEAR );

	frameRate = new IGLUFrameRate( 50 );

	Initialize();        // Run user-defined GL code needed prior to drawing.
}

void IGLUWindow::SetWidgetWindow( IGLUWidgetWindow *widgets )
{
	m_widgetWindow = widgets;
}

void IGLUWindow::SetWindowProperties( unsigned int flags )
{
	// Save our flags for later
	m_igluWindowFlags = flags;

	// Convert into an FLTK window mode
	m_fltkMode  = FL_RGB;
	m_fltkMode |= ( m_igluWindowFlags & IGLU_WINDOW_DEPTH )       ? FL_DEPTH : 0;
	m_fltkMode |= ( m_igluWindowFlags & IGLU_WINDOW_STENCIL )     ? FL_STENCIL : 0;
	m_fltkMode |= ( m_igluWindowFlags & IGLU_WINDOW_BLEND )       ? FL_ALPHA : 0;
	m_fltkMode |= ( m_igluWindowFlags & IGLU_WINDOW_DOUBLE )      ? FL_DOUBLE : 0;
	m_fltkMode |= ( m_igluWindowFlags & IGLU_WINDOW_MULTISAMPLE ) ? FL_MULTISAMPLE : 0;

	// Set the appropriate window mode
	mode(m_fltkMode);

	// Set if we should redraw during our idle callback
	m_redrawOnIdle = ( m_igluWindowFlags & IGLU_WINDOW_REDRAW_ON_IDLE ? true : false );

	// Check other properties
	m_noResize     = (m_igluWindowFlags & IGLU_WINDOW_NO_RESIZE) ? true : false;
	m_noIdle       = (m_igluWindowFlags & IGLU_WINDOW_NO_IDLE) ? true : false;
	m_displayFPS   = (m_igluWindowFlags & IGLU_WINDOW_W_FRAMERATE) ? true : false;

	if (!m_noResize) 
		resizable( this );
}

void IGLUWindow::Run( void )
{
	// Turn off vertical sync, if the user was silly enough leave it
	//    enabled the last time they installed their graphics drivers.
	//    Currently, I only know how to do this on windows.
#if defined(WIN32) && defined(_MSC_VER)
	wglSwapIntervalEXT(0);
#endif

	Fl::run();
}

void IGLUWindow::SetPreprocessOnGLInit( void (*preprocess)( void ) )
{
	m_preprocessOnGLInit = preprocess;
}

void IGLUWindow::SetIdleCallback( void (*newIdle)( void ) )
{
	// Remember what our idle function is
	m_callbackIdle = newIdle;

	// Tell FLTK to create a new idle callback that calls our static method,
	//    with the pointer to the instantiation of the current IGLUWindow wo
	//    we can get the appropriate class data.
	if (!m_fltkIdleAdded && m_callbackIdle)
	{
		Fl::add_idle( IGLUWindow::DefaultIdleFunction, this );
		m_fltkIdleAdded = true;
	}
}

void IGLUWindow::SetDisplayCallback( void (*newDisplay)( void ) )
{
	m_callbackDisplay = newDisplay;
}

void IGLUWindow::SetKeyboardCallback( void (*newKeyboard)( unsigned char, int, int ) )
{
	m_callbackKeyboard = newKeyboard;
}

void IGLUWindow::SetReshapeCallback( void (*newReshape)( int, int ) )
{
	m_callbackReshape = newReshape;
}

void IGLUWindow::SetMouseButtonCallback( void (*newButton)( int, int, int, int ) )
{
	m_callbackButton = newButton;
}

void IGLUWindow::SetActiveMotionCallback( void (*newActive)( int, int ) )
{
	m_callbackActiveMove = newActive;
}

void IGLUWindow::SetPassiveMotionCallback( void (*newPassive)( int, int ) )
{
	m_callbackPassiveMove = newPassive;
}

void IGLUWindow::SetEntryCallback( void (*newEntry)( int ) )
{
	m_callbackEntry = newEntry;
}

void IGLUWindow::SetVisibilityCallback( void (*newVisible)( int ) )
{
	m_callbackVisible = newVisible;
}

void IGLUWindow::SetSpecialKeyCallback( void (*newSpecial) ( int, int, int ) )
{
	m_callbackSpecial = newSpecial;
}

void IGLUWindow::CallIdleFunction( void )
{
	// If we don't have an idle function, what are we doing??
	if (m_callbackIdle) 
		(*m_callbackIdle)();

	// Should we redraw the screen after idling?
	if (m_redrawOnIdle)
		redraw();
}

// A static method that handles an idle() event, takes the input
//     void * pointer, which is actually a 'this' pointer to the
//     window that should be getting the idle event.
void IGLUWindow::DefaultIdleFunction ( void *igluWinPtr )
{
	if (!igluWinPtr) 
		return;
	
	((IGLUWindow *)igluWinPtr)->CallIdleFunction();
}

// A null function/method typically used for simple idling
void IGLUWindow::NullIdle( void )
{
}
