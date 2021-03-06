libefgy-8: README
=================

libefgy is a header-only C++ template library with lots of templates
dealing with maths in all its glory. And a few random other things not
directly related to maths that still come in handy.

OBTAINING LIBEFGY
-----------------

The library is distributed free of charge under a very permissive
licence. The public GIT repository is at
https://github.com/ef-gy/libefgy - see the documentation at
https://ef.gy/documentation/libefgy for more details.

INSTALLATION
------------

This is a header-only C++ template library. As such, installation is
rather simple: just copy the header files (the include/ef.gy directory)
to your compiler's default include path (typically /usr/include).

If you don't want to copy the files yourself, then run the following
command:

::

    $ sudo make PREFIX=/usr install

Adjust PREFIX as appropriate; substitute sudo when necessary.

DOCUMENTATION
-------------

libefgy uses Doxygen - http://www.doxygen.org/ - to document the code
right in the headers themselves. If you're stuck, just open up the
header file and read the documentation there. You can also extract an
HTML version of the documentation from a local source tree by running:

::

    $ make documentation

This will create or update the documentation in the directory
documentation/html, so that you can then just open the file
documentation/html/index.xhtml in a browser.

The HTML documentation for the current development version is also
available online at: https://ef.gy/documentation/libefgy

LICENCE
-------

libefgy is distributed under an MIT/X style licence. For all practical
intents and purposes that means that you can do pretty much whatever you
want with the code, except claim that you wrote it yourself, as long as
you point out somewhere in your documentation that you used libefgy.

See the file COPYING in the repository for the exact licence terms.
