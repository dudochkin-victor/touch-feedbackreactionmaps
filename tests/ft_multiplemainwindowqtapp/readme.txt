This is a simple test application to test multiple reaction maps in a
single application. The application creates two top-level windows
with a buttons labeled "Feedbacks" and "Transparent". The application
then switches between the windows in 2 second intervals.

Expected results:
- When "Feedbacks" is visible, tapping on the left side of the sceen
  should result in a press and release haptic feedback. Tapping on the
  right side of the screen should only result in press feedback.
- When "Transparent" is visible, tapping on the far left or far right
  side of the screen should result in no feedback. Tapping on the center
  region of the screen should work as if "Feedbacks" was visible.

To run the application, simply run "ft_multiplemainwindowqtapp" application
from the command line. To end the program, press ctrl+c.
