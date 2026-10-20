# OurPaint Desktop UI — Lib + App

Разделение исходного проекта на переиспользуемую библиотеку `OurPaint::Lib` (цель CMake `OurPaintLib`) и пример `OurPaintApp`. Сохранена исходная логика кастомного titlebar, кнопок окна, platform-specific resize/drag и FBO/texture viewport.

## Сборка (Windows, Visual Studio / CMake)

Требуется CMake 3.16+, C++17, Git и установленный компилятор C++ (например Visual Studio 2022). Зависимости GLFW 3.4 и Dear ImGui 1.90.9 загружаются через FetchContent при конфигурации, поэтому первый запуск требует интернет.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
.\build\Release\OurPaintApp.exe
```

Для Ninja/MinGW используйте обычные `cmake -S . -B build` и `cmake --build build`.

## Архитектура

- Корневой `CMakeLists.txt` содержит только настройки проекта и `add_subdirectory(Lib)` / `add_subdirectory(App)`.
- Публичный API библиотеки расположен в namespace `OurPaint`.

- `Lib/src/OurPaint.h` — публичный API.
- `Lib/src/` — окно, titlebar, FBO/texture, платформенная логика.
- `App/main.cc` — пример пользовательского приложения.
- `OurPaint::setViewportRenderer(callback)` передаёт библиотеке рендерер приложения. Callback выполняется при активном OpenGL-контексте и привязанном FBO. Не вызывайте `glfwSwapBuffers` внутри callback.
- Если callback не задан, Lib показывает встроенный анимированный fallback.

Пример callback в App можно заменить собственной 3D-отрисовкой. Размеры callback передаются в пикселях framebuffer. Приложение и библиотека работают в одном процессе и используют один OpenGL-контекст.

## Ограничения

В текущей схеме OpenGL-контекст создаёт Lib, а App получает возможность рисовать в предоставленный FBO. Это текстурное встраивание, не отдельный нативный дочерний HWND. Для большинства рендереров, использующих текущий контекст, это простой вариант; собственный отдельный контекст приложения потребует отдельной схемы shared contexts.
