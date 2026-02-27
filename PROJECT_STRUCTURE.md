# 📁 Структура Проекта KiOS

**Версия:** 2.0  
**Цель:** Чистая, организованная структура без мусора

---

## 🗂️ Общая Структура

```
KiOS/
├── src/                    # Исходный код ядра
│   ├── kernel/             # Главное ядро
│   │   ├── kernel.c       # Точка входа
│   │   ├── kernel.h
│   │   └── main.c
│   │
│   ├── arch/              # Архитектурно-зависимый код
│   │   ├── x86_64/        # x86_64 специфичные
│   │   │   ├── gdt/       # Global Descriptor Table
│   │   │   ├── idt/       # Interrupt Descriptor Table
│   │   │   ├── paging/    # VMM и paging
│   │   │   ├── syscall/   # System calls
│   │   │   └── asm/       # Ассемблерные вставки
│   │   │
│   │   └── common/        # Общие для всех архитектур
│   │
│   ├── mm/                # Memory Management
│   │   ├── pmm.c          # Physical Memory Manager
│   │   ├── pmm.h
│   │   ├── vmm.c          # Virtual Memory Manager
│   │   ├── vmm.h
│   │   ├── heap.c         # Dynamic heap
│   │   └── heap.h
│   │
│   ├── driver/            # Драйверы устройств
│   │   ├── pci/           # PCI bus
│   │   ├── ahci/          # SATA/AHCI storage
│   │   ├── ps2/           # PS/2 keyboard/mouse
│   │   │   ├── keyboard.c
│   │   │   └── mouse.c
│   │   ├── vesa/          # VESA/VBE graphics
│   │   ├── rtc/           # Real Time Clock
│   │   └── driver.h       # Общий интерфейс
│   │
│   ├── fs/                # Файловые системы
│   │   ├── vfs.c          # Virtual File System
│   │   ├── vfs.h
│   │   ├── kifs/          # KiOS File System
│   │   │   ├── kifs.c
│   │   │   ├── kifs.h
│   │   │   ├── inode.c
│   │   │   └── journal.c
│   │   └── fat/           # FAT32 (опционально, для совместимости)
│   │
│   ├── gui/               # Графический интерфейс
│   │   ├── display/       # Display server
│   │   ├── window/        # Window manager
│   │   ├── compositor/     # Compositor
│   │   ├── widgets/       # UI виджеты
│   │   └── events/       # Обработка событий
│   │
│   ├── gfx/              # Графика (низкий уровень)
│   │   ├── framebuf.c    # Framebuffer
│   │   ├── 2d.c          # 2D примитивы
│   │   ├── font.c        # Рендеринг шрифтов
│   │   └── image.c       # Загрузка изображений
│   │
│   ├── net/              # Сеть (Фаза 9)
│   │   ├── ethernet/
│   │   ├── tcpip/
│   │   └── socket.c
│   │
│   ├── ipc/              # Межпроцессное взаимодействие
│   │   ├── message.c
│   │   ├── semaphore.c
│   │   └── mutex.c
│   │
│   ├── process/          # Управление процессами
│   │   ├── process.c
│   │   ├── thread.c
│   │   ├── scheduler.c
│   │   └── elf.c         # ELF loader
│   │
│   ├── syscall/          # System call handlers
│   │   ├── syscall.c
│   │   └── syscalls/
│   │       ├── file.c
│   │       ├── memory.c
│   │       └── process.c
│   │
│   ├── lib/              # Внутренняя библиотека
│   │   ├── string.c
│   │   ├── printf.c
│   │   ├── malloc.c
│   │   └── utils.c
│   │
│   ├── include/          # Заголовочные файлы
│   │   ├── kios.h        # Главный API
│   │   ├── error.h       # Коды ошибок
│   │   └── types.h       # Базовые типы
│   │
│   └──boot/              # Загрузка (если отдельно)
│
├── kernel/                # Скомпилированное ядро
│   └── kiOS.bin
│
├── iso/                  # Образы для загрузки
│   ├── kiOS.iso
│   └── kiOS.img
│
├── userspace/            # Пользовательские программы
│   ├── kish/            # Командная оболочка
│   ├── kcalc/           # Калькулятор
│   ├── kedit/           # Редактор
│   ├── kimg/            # Просмотрщик
│   ├── kfile/           # Файловый менеджер
│   ├── kdesktop/        # Графическая оболочка
│   └── templates/       # Шаблоны для новых программ
│
├── tools/                # Инструменты сборки
│   ├── cross-compile.sh  # Настройка кросс-компиляции
│   ├── iso.sh           # Создание ISO
│   ├── run.sh           # Запуск в QEMU
│   └── debug/           # Отладочные скрипты
│
├── docs/                 # Документация
│   ├── architecture/    # Архитектурные решения
│   ├── api/             # API документация
│   ├── standards/       # Стандарты форматов
│   └── notes/          # Заметки разработки
│
├── tests/               # Тесты
│   ├── unit/            # Юнит-тесты
│   ├── integration/     # Интеграционные
│   └── stress/          # Нагрузочные
│
├── assets/              # Ресурсы
│   ├── fonts/           # Шрифты (.kfont)
│   ├── icons/           # Иконки
│   ├── wallpapers/      # Обои рабочего стола
│   └── sounds/          # Системные звуки
│
├── compiler/            # Компилятор KiLang (Фаза 7)
│   ├── kilang/          # Язык
│   │   ├── scanner/
│   │   ├── parser/
│   │   ├── ast/
│   │   ├── sema/        # Semantic analysis
│   │   ├── codegen/
│   │   └── llvm/        # or custom IR
│   │
│   └── libkios/         # Стандартная библиотека
│       ├── std/
│       ├── collections/
│       └── os/
│
├── .vscode/            # Настройки VS Code
│   ├── tasks.json
│   ├── launch.json
│   └── c_cpp_properties.json
│
├── Makefile            # Главный Makefile
├── README.md
├── CHANGELOG.md
├── LICENSE
└── .gitignore
```

---

## 📄 Описание Директорий

### `src/` — Исходный код

| Папка | Содержание |
|-------|------------|
| `arch/x86_64/` | CPU-специфичный код (GDT, IDT, paging) |
| `mm/` | Управление памятью (PMM, VMM, heap) |
| `driver/` | Драйверы устройств |
| `fs/` | Файловые системы |
| `gui/` | Графический интерфейс |
| `gfx/` | Низкоуровневая графика |
| `process/` | Процессы и планировщик |
| `syscall/` | Обработчики системных вызовов |
| `lib/` | Внутренняя библиотека ядра |

### `userspace/` — Пользовательские приложения

Каждая программа — в отдельной папке со своей структурой:

```
kcalc/
├── src/
│   └── main.c
├── Makefile
├── README.md
└── resources/
```

### `compiler/` — Тулчейн

```
compiler/
├── kilang/          # Компилятор
│   ├── src/         # Исходники
│   ├── tests/       # Тесты
│   └── Makefile
│
└── libkios/         # Стандартная библиотека
    ├── std/
    ├── collections/
    └── tests/
```

---

## 🔧 Скрипты Сборки

### Основные команды

```bash
make            # Собрать ядро
make iso        # Создать загрузочный ISO
make run        # Запустить в QEMU
make debug      # Отладка в QEMU+GDB
make clean      # Очистить
make userspace  # Собрать все приложения
make docs       # Собрать документацию
```

---

## 📋 Правила Именования

### Файлы

| Тип | Пример |
|-----|--------|
| Исходный код | `kernel.c`, `vmm.c` |
| Заголовочный | `kernel.h`, `vmm.h` |
| Тесты | `vmm_test.c` |
| Ассемблер | `boot.asm` |

### Функции

| Тип | Пример |
|-----|--------|
| Внутренние | `static void pmm_init(void)` |
| Экспортируемые | `void* kios_malloc(size_t)` |
| Макросы | `#define KMALLOC kios_malloc` |

### Переменные

| Тип | Пример |
|-----|--------|
| Глобальные | `static uint8_t* bitmap;` |
| Локальные | `uint64_t page_count;` |
| Константы | `static const uint64_t PAGE_SIZE = 4096;` |

---

## ✅ Чеклист Новой Компоненты

При добавлении новой подсистемы:

- [ ] Создать папку в `src/`
- [ ] Добавить заголовочный файл `.h`
- [ ] Добавить реализацию `.c`
- [ ] Обновить `Makefile`
- [ ] Добавить в `docs/` если нужно
- [ ] Добавить тесты в `tests/`

---

## 🚀 Быстрый Старт

```bash
# Клонирование и сборка
git clone <repo> KiOS
cd KiOS
make

# Запуск
make run

# Отладка
make debug
```

---

*Структура соответствует принципу: всё на своём месте.*
