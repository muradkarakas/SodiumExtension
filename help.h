/*! \file   help.h
    \brief  Generic properties of HT/SQL application

    \n
    \n
    1.  Only one "*.frmx" file can reside in server memory for each session. AJAX/POST requests from this session are processed with using these memory instances.
        If another GET/PAGE request is made for a different mk file from same session but before closing the first mk file's browser window,
        firs mk file memory objects will be destroyed and further AJAX/POST requests are broken. Probably will cause IIS worker process to halt.
        This is not the case for different sessions. One session requests (whether it is GET or POST, is not interfere with another session GET/POST requests.
    \n
    \n
    2.  Thread safety is only implied for GET/PAGE requests from all sessions. In other word, AJAX/POST requests are not protected in terms of thread safety.
        All pages requests wait until other page request done. So, pages should be lighter as much as possible in order to avoid race for resources.
    \n
    \n
    3.

*/

#ifndef HELP_H_INCLUDED
#define HELP_H_INCLUDED



#endif // HELP_H_INCLUDED
