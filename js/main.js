// ========================================
// KiOS Documentation - JavaScript
// ========================================

// Копирование кода в буфер обмена
function copyCode(button) {
    const codeBlock = button.closest('.code-block');
    const code = codeBlock.querySelector('code').textContent;
    
    navigator.clipboard.writeText(code).then(() => {
        const originalText = button.textContent;
        button.textContent = '✅ Скопировано!';
        button.style.color = 'var(--accent-green)';
        
        setTimeout(() => {
            button.textContent = originalText;
            button.style.color = '';
        }, 2000);
    });
}

// Мобильное меню (для будущего использования)
function toggleMobileMenu() {
    const sidebar = document.querySelector('.sidebar');
    sidebar.classList.toggle('mobile-open');
}

// Подсветка активного пункта меню
function setActiveNavItem() {
    const currentPath = window.location.pathname;
    const navItems = document.querySelectorAll('.nav-item');
    
    navItems.forEach(item => {
        if (item.getAttribute('href') === currentPath.split('/').pop()) {
            item.classList.add('active');
        }
    });
}

// Инициализация при загрузке
document.addEventListener('DOMContentLoaded', () => {
    setActiveNavItem();
    
    // Добавляем классы для анимации
    const cards = document.querySelectorAll('.feature-card, .doc-card, .phase-card');
    cards.forEach((card, index) => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(10px)';
        
        setTimeout(() => {
            card.style.transition = 'all 0.3s ease';
            card.style.opacity = '1';
            card.style.transform = 'translateY(0)';
        }, index * 50);
    });
});

// Обработка горячих клавиш
document.addEventListener('keydown', (e) => {
    // Ctrl/Cmd + K для поиска (будущее)
    if ((e.ctrlKey || e.metaKey) && e.key === 'k') {
        e.preventDefault();
        // Можно добавить поиск
    }
});

// Плавная прокрутка для якорных ссылок
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    });
});

// Тёмная тема уже по умолчанию (CSS handles it)
console.log('📘 KiOS Documentation loaded');
