// This is template file for clnk program
// Note that generator will add "#define APPLICATION_PATH ..." in this file
// If working directory was been specified, generator also will define APPLICATION_WORKING_DIRECTORY

// comment this line to no use unicode
#define UNICODE

#ifdef UNICODE
#define _ENTRY_POINT_ wWinMain
#else
#define _ENTRY_POINT_ WinMain
#endif

#include <sstream>

#include <Windows.h>

TCHAR* get_command_line_arguments(TCHAR* command_line) {
  TCHAR quotes_end_char = 0;

  for (; *command_line && (*command_line != ' ' || quotes_end_char); ++command_line) {
    if (quotes_end_char && *command_line == quotes_end_char)
      quotes_end_char = 0;
    else if (*command_line == '"' || *command_line == '\'')
      quotes_end_char = *command_line; // invert
  }

  return *command_line ? command_line : nullptr;
}

void show_process_creation_error() {
  DWORD error_code = GetLastError();

  std::stringstream message_box_text;

  message_box_text <<
    "Failed to create process \"" APPLICATION_PATH "\"\n"
#ifdef APPLICATION_WORKING_DIRECTORY
    "Working directory: \"" APPLICATION_WORKING_DIRECTORY "\"\n"
#else
    "Working directory not specified\n"
#endif
    "\n"
    "Error code: " << std::hex << error_code;

  auto message_box_text_string = message_box_text.str();

  MessageBoxA(NULL, message_box_text_string.c_str(), "Link to " APPLICATION_PATH, MB_ICONERROR);
}

bool try_create_process_with_elevation(TCHAR* arguments) {
  return ShellExecute(
    NULL,
    TEXT("runas"),
    TEXT(APPLICATION_PATH),
    arguments,
#ifdef APPLICATION_WORKING_DIRECTORY
    TEXT(APPLICATION_WORKING_DIRECTORY),
#else
    NULL,
#endif
    TRUE
  );
}

bool try_create_process(TCHAR* arguments) {
  bool success = ShellExecute(
    NULL,
    TEXT("open"),
    TEXT(APPLICATION_PATH),
    arguments,
#ifdef APPLICATION_WORKING_DIRECTORY
    TEXT(APPLICATION_WORKING_DIRECTORY),
#else
    NULL,
#endif
    TRUE
  );

  if (!success) {
    DWORD error_code = GetLastError();

    if (error_code == ERROR_ELEVATION_REQUIRED)
      return try_create_process_with_elevation(arguments);
    else
      return false;
  }

  return true;
}

int __stdcall _ENTRY_POINT_(HINSTANCE instance, HINSTANCE previous_instance, LPTSTR command_line, int show_command) {
  auto arguments = get_command_line_arguments(command_line);

  if (try_create_process(arguments))
    return 0;
  else {
    show_process_creation_error();
    return -1;
  }
}