# 📘 KiLang — Спецификация Языка

**Версия:** 0.1 (Черновик)  
**Статус:** Проектирование

---

## 🎯 Цель

KiLang — системный язык программирования, разработанный для:

1. **Эффективности** — минимум накладных расходов
2. **Безопасности** — встроенная защита от ошибок
3. **Простоты** — читаемый, понятный синтаксис
4. **Интеграции** — прямой доступ к возможностям ядра KiOS

---

## 🏗️ Философия

```
"Язык, который понимает систему."
```

- Никаких лишних слоёв абстракции
- Прямой доступ к памяти там, где нужно
- Безопасность по умолчанию
- Компиляция в нативный код

---

## 📝 Основы

### Привет, Мир

```kotlin
// KiLang - Hello World
fn main() {
    println("Привет, KiOS!");
}
```

### Переменные

```kotlin
// Неизменяемая (рекомендуется)
let x: i32 = 42;
let name: str = "KiOS";

// Изменяемая (если нужно)
let mut counter: u64 = 0;
counter = counter + 1;

// Типизация выводится компилятором
let inferred = 123;        // i32 по умолчанию
let text = "hello";        // str
```

### Типы данных

#### Примитивные

| Тип | Размер | Описание |
|-----|--------|----------|
| `i8`, `i16`, `i32`, `i64` | 8-64 bit | Знаковые целые |
| `u8`, `u16`, `u32`, `u64` | 8-64 bit | Беззнаковые целые |
| `f32`, `f64` | 32-64 bit | Числа с плавающей точкой |
| `bool` | 8 bit | true / false |
| `char` | 8 bit | Unicode символ |
| `str` | N/A | Строка (utf-8) |
| `void` | 0 | Отсутствие значения |

#### Составные

```kotlin
// Массив
let arr: [i32; 5] = [1, 2, 3, 4, 5];
let slice: []i32 = arr[0..3];  // Срез

// Кортеж
let point: (i32, i32) = (10, 20);
let (x, y) = point;

// Структура
struct Point {
    x: i32,
    y: i32,
}

let p = Point { x: 10, y: 20 };

// Перечисление
enum Result<T, E> {
    Ok(T),
    Err(E),
}

// Объединение (unsafe)
union Payload {
    int: i32,
    float: f32,
    bytes: [u8; 4],
}
```

### Операторы

#### Арифметические

```kotlin
+   // Сложение
-   // Вычитание
*   // Умножение
/   // Деление
%   // Остаток
**  // Возведение в степень
```

#### Побитовые

```kotlin
&   // AND
|   // OR
^   // XOR
<<  // Сдвиг влево
>>  // Сдвиг вправо
~   // Инверсия
```

#### Сравнения

```kotlin
==  // Равно
!=  // Не равно
<   // Меньше
>   // Больше
<=  // Меньше или равно
>=  // Больше или равно
```

#### Логические

```kotlin
&&  // AND
||  // OR
!   // NOT
```

---

## 🔄 Управление Потоком

### Условия

```kotlin
if x > 0 {
    println("positive");
} else if x < 0 {
    println("negative");
} else {
    println("zero");
}

// Тернарный оператор
let sign = if x >= 0 { "+" } else { "-" };
```

### Циклы

```kotlin
// while
while count > 0 {
    count = count - 1;
}

// for (по диапазону)
for i in 0..10 {
    print(i);
}

// for (по итерируемому)
for item in items {
    process(item);
}

// Бесконечный цикл
loop {
    // ...
    if should_break {
        break;
    }
}

// Continue
for i in 0..10 {
    if i == 5 {
        continue;  // Пропустить 5
    }
    print(i);
}
```

### Match (Pattern Matching)

```kotlin
let result: Result<i32, str> = Ok(42);

match result {
    Ok(value) => println("Value: {}", value),
    Err(e)    => println("Error: {}", e),
}

// Сопоставление с охранением
match option {
    Some(x) if x > 10 => println("big"),
    Some(_)           => println("small"),
    None              => println("none"),
}

// Match как выражение
let description = match num {
    0 => "zero",
    1 => "one",
    2 => "two",
    _ => "other",
};
```

---

## 📦 Функции

### Определение

```kotlin
// Простая функция
fn greet(name: str) {
    println("Hello, {}!", name);
}

// С возвратом
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

// Вывод типа возврата
fn multiply(a: i32, b: i32) -> i32 {
    a * b  // Последнее выражение - возвращаемое
}

// Функции высшего порядка
fn apply<T, R>(value: T, func: fn(T) -> R) -> R {
    func(value)
}
```

### Параметры

```kotlin
// Значение
fn foo(x: i32) { ... }

// Указатель (изменяемый)
fn bar(ptr: *i32) {
    *ptr = 10;
}

// Ссылка
fn baz(ref: &i32) {
    let x = *ref;
}

//可变引用
fn qux(ref: &mut i32) {
    *ref = *ref + 1;
}

// Variadic
fn printf(format: str, args: ...) {
    // ...
}
```

---

## 🏛️ Модули

### Импорт

```kotlin
// Импорт стандартной библиотеки
use std::io;
use std::fmt;

// Импорт локального модуля
use "./utils.kl";

// Импорт с алиасом
use std::io as ios;
```

### Определение модуля

```kotlin
// math.kl
pub fn sqrt(x: f64) -> f64 { ... }
pub const PI: f64 = 3.14159;

// main.kl
use math;

fn main() {
    let r = math::sqrt(2.0) * math::PI;
}
```

---

## 🔒 Безопасность

### Ownership (Владение)

```kotlin
// Перемещение (move)
let s1 = String::new("hello");
let s2 = s1;  // s1 больше не действителен

// Клонирование
let s2 = s1.clone();

// Заимствование (borrow)
let s3 = &s1;        // Неизменяемая ссылка
let s4 = &mut s1;   // Изменяемая ссылка
```

### Option и Result

```kotlin
// Option - значение может отсутствовать
fn find_user(id: u32) -> Option<User> {
    if exists(id) {
        Some(get_user(id))
    } else {
        None
    }
}

// Result - значение или ошибка
fn read_file(path: str) -> Result<FileData, Error> {
    if !exists(path) {
        return Err(Error::NotFound);
    }
    Ok(read(path))
}

// Обработка с ?
let user = find_user(123)?;  // Паника если None
let data = read_file("a.kl")?;
```

### Проверка границ

```kotlin
let arr = [1, 2, 3, 4, 5];

// Безопасный доступ (возвращает Option)
let val = arr.get(10);  // None

// Проверенный доступ
let val = arr[10]?;     // Паника если out of bounds
```

---

## ⚡ unsafe

```kotlin
// Когда нужен низкоуровневый доступ
unsafe {
    // Прямой доступ к памяти
    let ptr = 0x1000 as *mut i32;
    *ptr = 42;
    
    // Вызов внешнего кода
    extern "C" {
        fn system(cmd: *const u8) -> i32;
    }
}
```

---

## 📋 Стандартная Библиотека (libkios)

### std::io

```kotlin
use std::io;

// Вывод
print("text");
println("text with newline");
eprintln("to stderr");

// Форматирование
let x = 42;
println("Value: {}", x);
println("Hex: {:x}", x);
println("Float: {:.2}", 3.14159);

// Ввод
let line = io::read_line();
let num: i32 = io::read().parse().unwrap();
```

### std::fs

```kotlin
use std::fs;

// Чтение файла
let content = fs::read_to_string("file.kl");

// Запись
fs::write("output.txt", "data");

// Работа с путями
let path = Path::new("/home/user/file.txt");
let ext = path.extension();
```

### std::collections

```kotlin
use std::collections::*;

// Динамический массив
let mut vec = Vec::new();
vec.push(1);
vec.push(2);
let first = vec[0];

// Хэш-карта
let mut map = HashMap::new();
map.insert("key", "value");
let val = map.get("key");

// Строка
let mut s = String::new();
s.push_str("Hello");
s.push('!');
```

### std::net

```kotlin
use std::net;

// TCP сервер
let listener = net::TcpListener::bind("127.0.0.1:8080");
for stream in listener.accept() {
    // обработка
}

// HTTP запрос
let response = net::http::get("https://kios.os");
```

---

## 🔧 FFI (Foreign Function Interface)

### Вызов функций ядра

```kotlin
// Автоматический FFI
use kios;

// Вызов syscall
let fd = kios::open("/dev/fb0", O_RDWR);
kios::write(fd, buffer, size);
kios::close(fd);

// Работа с памятью
let ptr = kios::mmap(null, 4096, PROT_READ | PROT_WRITE);
```

---

## 📊 Сравнение с Другими Языками

| Feature | KiLang | Rust | C | Go |
|---------|---------|------|---|-----|
| Ownership | ✅ | ✅ | ❌ | ❌ |
| Null safety | ✅ | ✅ | ❌ | ✅ |
| Pattern match | ✅ | ✅ | ❌ | ✅ |
| Generics | ✅ | ✅ | ❌ | ❌ |
| unsafe | ✅ | ✅ | ✅ | ❌ |
| Garbage collection | ❌ | ❌ | ❌ | ✅ |
| VM/Interpreter | ❌ | ❌ | ❌ | ❌ |
| Native binary | ✅ | ✅ | ✅ | ✅ |

---

## 🚀 Примеры

### Работа с Файлами

```kotlin
use std::fs;
use std::io;

fn main() {
    let args = os::args();
    
    if args.len() < 2 {
        eprintln("Usage: cat <file>");
        return;
    }
    
    let path = args[1];
    match fs::read_to_string(path) {
        Ok(content) => print(content),
        Err(e)      => eprintln!("Error: {}", e),
    }
}
```

### HTTP Сервер

```kotlin
use std::net;
use std::io;

fn handle_request(req: &str) -> str {
    "HTTP/1.1 200 OK\r\n\r\nHello, KiOS!"
}

fn main() {
    let listener = net::TcpListener::bind("0.0.0.0:8080");
    println!("Server started on port 8080");
    
    for conn in listener {
        let request = conn.read();
        let response = handle_request(request);
        conn.write(response);
    }
}
```

### GUI Приложение

```kotlin
use kios::gui::*;

fn main() {
    let window = Window::new("My App", 800, 600);
    
    let button = Button::new("Click me!", 10, 10, 100, 30);
    button.on_click(|| {
        println!("Button clicked!");
    });
    
    window.add(button);
    window.run();
}
```

---

## 📅 План Реализации

1. **Lexer/Parser** — разбор исходного кода
2. **AST** — построение дерева
3. **Semantic Analysis** — проверка типов
4. **Code Gen** — генерация KiELF
5. **Standard Lib** — базовая библиотека
6. **Self-hosting** — компилятор на KiLang

---

*Спецификация находится в разработке и может измениться.*
