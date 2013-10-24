#include <stdio.h>
#include <stdlib.h>

#include "iglu.h"


#if defined _WIN32 || defined _WIN64
        #include <GL/wglew.h>
#else
        #include <GL/glxew.h>
#endif


using namespace iglu;

// Initialization of static variables for IGLUIMainWindow

void (*IGLUMainWindow::m_callbackDisplay)( void )						    = NULL;
void (*IGLUMainWindow::m_callbackReshape)( int, int )					    = NULL;
void (*IGLUMainWindow::m_callbackKeyboard)( unsigned char, int, int )	    = NULL;
void (*IGLUMainWindow::m_callbackSpecial)( int, int, int )				    = NULL;
void (*IGLUMainWindow::m_callbackButton)( int, int, int, int )			    = NULL;
void (*IGLUMainWindow::m_callbackActiveMove)( int, int )					= NULL;
void (*IGLUMainWindow::m_callbackPassiveMove)( int, int )			    	= NULL;
void (*IGLUMainWindow::m_callbackEntry)( int )							    = NULL;
void (*IGLUMainWindow::m_callbackVisible)( int )							= NULL;
void (*IGLUMainWindow::m_callbackIdle)( void )							    = NULL;

void (*IGLUMainWindow::m_preprocessOnGLInit)( void )						= NULL;

bool			IGLUMainWindow::m_isRunning								    = false;
bool			IGLUMainWindow::m_isInitialized							    = false;
bool			IGLUMainWindow::m_isWindowInitialized					    = false;
int				IGLUMainWindow::m_currentWidth							    = 512;
int				IGLUMainWindow::m_currentHeight							    = 512;
int				IGLUMainWindow::m_initialPosX							    = 100; 
int				IGLUMainWindow::m_initialPosY							    = 100;
unsigned int	IGLUMainWindow::m_igluWindowFlags						    = IGLU_WINDOW_NO_FLAGS;

// Widget-specific variables
int				IGLUMainWindow::m_currentButtonDown						    = -1;




void IGLUMainWindow::Initialize( int *argc, char **argv )
{
	glutInit( argc, argv );
	m_isInitialized = true;
}

void IGLUMainWindow::SetWindowProperties( unsigned int flags )
{
	m_igluWindowFlags = flags;
}

void IGLUMainWindow::Run( const char *windowTitle, int windowWidth, int windowHeight )
{
	// Make sure GLUT has been initialized properly
	if (!m_isInitialized)
	{
		int noInitArgCount = 0;
		Initialize( &noInitArgCount, NULL );
	}

	// Initialize the GLUT display mode (based on IGLUIMainWindow class settings)
	unsigned int m_displayMode = 0;
	m_displayMode |= ( m_igluWindowFlags & IGLU_WINDOW_DEPTH )       ? GLUT_DEPTH : 0;
	m_displayMode |= ( m_igluWindowFlags & IGLU_WINDOW_STENCIL )     ? GLUT_STENCIL : 0;
	m_displayMode |= ( m_igluWindowFlags & IGLU_WINDOW_BLEND )       ? GLUT_ALPHA : 0;
	m_displayMode |= ( m_igluWindowFlags & IGLU_WINDOW_DOUBLE )      ? GLUT_DOUBLE : 0;
	m_displayMode |= ( m_igluWindowFlags & IGLU_WINDOW_MULTISAMPLE ) ? GLUT_MULTISAMPLE : 0;

	glutInitDisplayMode( m_displayMode );

	// Initialize the GLUT window size (based on method parameters)
	glutInitWindowSize( m_currentWidth = windowWidth, m_currentHeight = windowHeight );

	// Initialize the GLUT window position 
	glutInitWindowPosition( m_initialPosX, m_initialPosY );

	// Create the GLUT window, with title from method parameters
	glutCreateWindow( windowTitle );

	// Initialize GLEW
	glewInit();

	// Create a widget dock to hold any user widgets
	m_isWindowInitialized = true;

	// Turn off vertical sync, if the user was silly enough leave it
	//    enabled the last time they installed their graphics drivers.
	//    Currently, I only know how to do this on windows.
#if defined(WIN32) && defined(_MSC_VER)
	wglSwapIntervalEXT(0);
#endif

	// Do any preprocessing required prior to calling glutMainLoop()
	if (m_preprocessOnGLInit)
		(*m_preprocessOnGLInit)();

	// Now setup the appropriate callback functions.  Note that these callbacks are simply
	//    hooks, and they actually call the user-specified callbacks after (possibly)
	//    injecting window-widget interactions.
	glutDisplayFunc( IGLUMainWindow::GlobalDisplayCallback );
	glutReshapeFunc( IGLUMainWindow::GlobalReshapeCallback );
	glutIdleFunc( IGLUMainWindow::GlobalIdleFunction );
	glutMouseFunc( IGLUMainWindow::GlobalMouseButtonCallback );
	glutMotionFunc( IGLUMainWindow::GlobalActiveMotionCallback );
	glutPassiveMotionFunc( IGLUMainWindow::GlobalPassiveMotionCallback );
	glutKeyboardFunc( IGLUMainWindow::GlobalKeyboardCallback );
	glutSpecialFunc( IGLUMainWindow::GlobalSpecialKeyCallback );
	glutEntryFunc( IGLUMainWindow::GlobalEntryCallback );
	glutVisibilityFunc( IGLUMainWindow::GlobalVisibilityCallback );

	// Finally, start the OpenGL program!
	m_isRunning = true;
	glutMainLoop();
}

void IGLUMainWindow::SetPreprocessOnGLInit( void (*preprocess)( void ) )
{
	m_preprocessOnGLInit = preprocess;
}

void IGLUMainWindow::SetDisplayCallback( void (*newDisplay)( void ) )
{
	m_callbackDisplay = newDisplay;
}
void IGLUMainWindow::SetReshapeCallback( void (*newReshape)( int, int ) )
{
	m_callbackReshape = newReshape;
}
void IGLUMainWindow::SetKeyboardCallback( void (*newKeyboard)( unsigned char, int, int ) )
{
	m_callbackKeyboard = newKeyboard;
}
void IGLUMainWindow::SetSpecialKeyCallback( void (*newSpecial) ( int, int, int ) )
{
	m_callbackSpecial = newSpecial;
}
void IGLUMainWindow::SetMouseButtonCallback( void (*newButton)( int, int, int, int ) )
{
	m_callbackButton = newButton;
}
void IGLUMainWindow::SetActiveMotionCallback( void (*newActive)( int, int ) )
{
	m_callbackActiveMove = newActive;
}
void IGLUMainWindow::SetPassiveMotionCallback( void (*newPassive)( int, int ) )
{
	m_callbackPassiveMove = newPassive;
}
void IGLUMainWindow::SetEntryCallback( void (*newEntry)( int ) )
{
	m_callbackEntry = newEntry;
}
void IGLUMainWindow::SetVisibilityCallback( void (*newVisible)( int ) )
{
	m_callbackVisible = newVisible;
}
void IGLUMainWindow::SetIdleCallback( void (*newIdle)( void ) )
{
	m_callbackIdle = newIdle;
}

void IGLUMainWindow::GlobalDisplayCallback( void )
{
	if (m_callbackDisplay) (*m_callbackDisplay)();	

	// Let the widgets redraw themselves
	//widgets->Redraw();

	/* flush GL commands & swap buffers */
	glFlush();
	if (m_igluWindowFlags & IGLU_WINDOW_DOUBLE)
		glutSwapBuffers();
}

void IGLUMainWindow::GlobalReshapeCallback( int width, int height )
{
	// If user specified the window to be non-resizable, make it so
	if ( (m_igluWindowFlags & IGLU_WINDOW_NO_RESIZE) &&
		 (m_currentWidth != width || m_currentHeight != height) )
	{
		glutReshapeWindow( m_currentWidth, m_currentHeight );
		return;
	}

	// Call the user's reshape function
	if (m_callbackReshape) (*m_callbackReshape)(width,height);
}

void IGLUMainWindow::GlobalKeyboardCallback( unsigned char key, int x, int y )
{
	if (m_callbackKeyboard) (*m_callbackKeyboard)(key,x,y);
}

void IGLUMainWindow::GlobalSpecialKeyCallback( int key, int x, int y )
{
	if (m_callbackSpecial) (*m_callbackSpecial)(key,x,y);
}

void IGLUMainWindow::GlobalMouseButtonCallback( int button, int state, int x, int y )
{
	// Check if any on-screen widgets got the mouse click/release
	/*
	bool widgetCaught=false;
	if ( state == GLUT_DOWN )
		widgetCaught = widgets->OnMouseClick( button, x, y );	
	else 
		widgets->OnMouseRelease( button, x, y );
		*/

	// If not, then call the user-registered callback
	if (m_callbackButton) (*m_callbackButton)(button,state,x,y);

	// Remember the last button we saw!
	m_currentButtonDown = button;
}

void IGLUMainWindow::GlobalActiveMotionCallback( int x, int y )
{
	//widgets->OnActiveMouseMove( currentButtonDown, x, y );	

	if (m_callbackActiveMove) (*m_callbackActiveMove)(x,y);
}

void IGLUMainWindow::GlobalPassiveMotionCallback( int x, int y )
{
	//widgets->OnPassiveMouseMove( x, y );	

	if (m_callbackPassiveMove) (*m_callbackPassiveMove)(x,y);
}

void IGLUMainWindow::GlobalEntryCallback( int state )
{
	if (m_callbackEntry) (*m_callbackEntry)(state);
}

void IGLUMainWindow::GlobalVisibilityCallback( int state )
{
	if (m_callbackVisible) (*m_callbackVisible)(state);
}

void IGLUMainWindow::GlobalIdleFunction ( void )
{
	if (m_callbackIdle) (*m_callbackIdle)();

	glutPostRedisplay();
}
