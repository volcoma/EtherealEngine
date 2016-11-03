tracey/redist
=============

- This optional folder is used to regenerate the amalgamated distribution. Do not include it into your project.
- Regenerate the distribution by typing the following lines:
```
move /y tracey.hpp .. 
deps\Amalgamate.exe -p tracey.hpp -w "*.*pp;*.c;*.h" tracey.cpp ..\tracey.cpp
deps\fart.exe -- ..\tracey.cpp "#line" "//#line"
deps\fart.exe -- ..\tracey.cpp "#pragma once" "//#pragma once"
copy /y ..\tracey.hpp
```
