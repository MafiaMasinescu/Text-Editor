#include <utility>
#include <QApplication>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QObject>
#include <QMenuBar>
#include <QAction>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QComboBox>
#include <QSettings>
#include <QProcess>
#include <QPalette>
#include <QListWidget>
#include <QStackedWidget>
#include <QRadioButton>
#include <QLineEdit>
#include <QStatusBar>
#include <QMenu>
#include <QFontMetrics>
#include <QTextBlock>
#include <QToolBar>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextFormat>
#include <QTextCursor>
#include <QResizeEvent>
#include <QTabBar>
#include <QMap>
#include <QStackedWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QInputDialog>
#include <QAbstractTextDocumentLayout>

int osdetection()
{
    int ostype = 0;
    #ifdef Q_OS_WIN
        ostype = 1; // Windows
    #elif defined(Q_OS_MACOS)
        ostype = 2; // macOS
    #elif defined(Q_OS_LINUX)
        ostype = 3; // Linux
    #else
        ostype = 0; // Unknown OS
    #endif
    return ostype;
}
int ostype = osdetection();

QString detectSystemTheme() {
    switch(ostype) {
        case 1: { // Windows
            QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                               QSettings::NativeFormat);

            if (settings.contains("AppsUseDarkTheme")) {
                int dark = settings.value("AppsUseDarkTheme").toInt();
                if(dark == 1)
                {

                    return "Dark";
                }

            }
            else if (settings.contains("AppsUseLightTheme")) {
                return settings.value("AppsUseLightTheme").toInt() == 1 ? "Light" : "DarkGray";
            }
            return "DarkGray";
        }
        case 2: { // macOS
            QSettings settings("Apple", "Global Preferences");
            return settings.value("AppleInterfaceStyle", "").toString() == "Dark" ? "Dark" : "Light";
        }
        case 3: { // Linux
            // pt GNOME - setari pt desktop pt linux/unix
            QProcess process;
            process.start("gsettings", QStringList() << "get" << "org.gnome.desktop.interface" << "gtk-theme");
            process.waitForFinished();
            QString output = process.readAllStandardOutput();
            return output.contains("dark", Qt::CaseInsensitive) ? "Dark" : "Light";
        }
        default:
            return "DarkGray";
    }
}

std::pair<int, int> text_lines_characters(const QByteArray& rawData) {
    int characterCount = rawData.size();
    int lineCount = 1;
    for (char c : rawData) {
        if (c == '\n') lineCount++;
    }
    int char_crlf = characterCount + lineCount - 1;;
    return std::make_pair(lineCount, char_crlf);
}

struct Theme {
    QString background;
    QString textColor;
    QString buttonColor;
    QString buttonBorder;
    QString buttonHover;
    QString textEditBg;
    QString borderColor;
};
const Theme DarkContrast {
    "#000000",  // background
    "#ffffff",  // textColor
    "#232323",  // buttonColor
    "#333333",  // buttonBorder
    "#2d2d2d",  // buttonHover
    "#000000",  // textEditBg
    "#333333"   // borderColor
};
const Theme DarkGray {
    "#1e1e1e",  // background
    "#ffffff",  // textColor
    "#1e1e1e",  // buttonColor
    "#666666",  // buttonBorder
    "#505050",  // buttonHover
    "#2b2b2b",  // textEditBg
    "#555555"   // borderColor
};
const Theme Light {
    "#ffffff",  // background
    "#000000",  // textColor
    "#f0f0f0",  // buttonColor
    "#cccccc",  // buttonBorder
    "#e5e5e5",  // buttonHover
    "#ffffff",  // textEditBg
    "#dddddd"   // borderColor
};
const Theme Solarized {
    "#002b36",  // background
    "#839496",  // textColor
    "#073642",  // buttonColor
    "#586e75",  // buttonBorder
    "#094352",  // buttonHover
    "#002b36",  // textEditBg
    "#586e75"   // borderColor
};
/// nu mai fac design sau css in viata mea 
QString createStyleSheet(const Theme& theme, int borderRadius = 6) {
    QString arrowImage = (theme.background == Light.background) ? ":/black_down_arrow.png" : ":/arrow_down.png";
    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-size: 14px;
        }
        /* Tab Bar Styling */
        QTabBar {
            background-color: %1;
            color: %2;
        }
        QTabBar::tab {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            border-bottom: none;
            padding: 6px 10px;
            margin-right: 2px;
            border-top-left-radius: %9px;
            border-top-right-radius: %9px;
        }
        QTabBar::tab:selected {
            background-color: %5;
            border-bottom: none;
        }
        QTabBar::tab:hover:!selected {
            background-color: %5;
        }
        QTabBar::close-button {
            image: url(:/close.png);
            subcontrol-position: center;
        }
        QTabBar::close-button:hover {
            background-color: %5;
        }
        QTextEdit {
            background-color: %6;
            color: %2;
            border: 1px solid %7;
            padding: 6px;
            border-radius: %9px;
        }
        QPushButton {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            padding: 6px;
            border-radius: %9px;
        }
        QPushButton:hover {
            background-color: %5;
        }
        QMenuBar {
            background-color: %1;
            color: %2;
        }
        QMenuBar::item:selected {
            background-color: %5;
        }
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %7;
        }
        QMenu::item:selected {
            background-color: %5;
        }
        QScrollBar:vertical {
            background-color: %1;
            width: 12px;
        }
        QScrollBar::handle:vertical {
            background-color: %3;
            border-radius: %9px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: %5;
        }
        QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {
            height: 0px;
            background: none;
        }
        QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical {
            background: %1;
        }
        QComboBox {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            padding: 6px;
            border-radius: %9px;
        }
        QComboBox:hover {
            background-color: %5;
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid %4;
        }
        QComboBox::down-arrow {
            image: url(%8);
            width: 15px;
            height: 15px;
        }
        QComboBox QAbstractItemView {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            selection-background-color: %5;
            selection-color: %2;
            outline: none;
        }
        QListWidget {
            background-color: %3;
            border: none;
            padding: 5px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: %9px;
        }
        QListWidget::item:selected {
            background-color: %5;
            color: %2;
        }
        QListWidget::item:focus {
            outline: none;
        }
        QListWidget::item:hover {
            background-color: %5;
        }
        QListWidget::item:!active {
            outline: none;
        }
        QStackedWidget {
            background-color: %6;
            border: 1px solid %7;
            border-radius: %9px;
            padding: 15px;
        }
        QStackedWidget:focus {
            outline: none;
            border: none;
        }
        QComboBox:focus, QComboBox QAbstractItemView:focus {
            outline: none;
            border: 1px solid %4;
        }
        QComboBox QAbstractItemView {
            outline: none;
        }
        QStatusBar QPushButton[flat="true"] {
            border: none;
            background: transparent;
            padding: 0 5px;
            color: %2;
        }
        QStatusBar QLabel {
            color: %2;
        }
        QStatusBar {
            background-color: %3;
            color: %2;
            border-left: 1px solid %7;
            border-right: 1px solid %7;
            border-bottom: 1px solid %7;
            border-top: 1px solid %7;
            border-bottom-left-radius: %9px;
            border-bottom-right-radius: %9px;
        }
        QStatusBar::item {
            border: none;
        } 
    )")
    .arg(theme.background)    // %1
    .arg(theme.textColor)     // %2
    .arg(theme.buttonColor)   // %3
    .arg(theme.buttonBorder)  // %4
    .arg(theme.buttonHover)   // %5
    .arg(theme.textEditBg)    // %6
    .arg(theme.borderColor)   // %7
    .arg(arrowImage)          // %8
    .arg(borderRadius);        // %9
}

Theme applySystemTheme(QApplication& app, int borderRadius = 6) {
    QString systemTheme = detectSystemTheme();
    if (systemTheme == "Dark") {
        app.setStyleSheet(createStyleSheet(DarkContrast, borderRadius));
        return DarkContrast;
    } else if (systemTheme == "Light") {
        app.setStyleSheet(createStyleSheet(Light, borderRadius));
        return Light;
    } else {
        app.setStyleSheet(createStyleSheet(DarkGray, borderRadius));
        return DarkGray;
    }
}
/// nu o sa ma angajeze nimeni vreodata daca programez totul intro singur fisier da nah am vrut si eu sa ma distrez o data
Theme currentTheme;
int radius = 6;
void settings(QMainWindow &window, QApplication &app) {
    QDialog *settingsDialog = new QDialog(&window);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->setWindowTitle("Settings");
    settingsDialog->setFixedSize(600, 400);
    QHBoxLayout *mainlayout = new QHBoxLayout();
    QListWidget *sidebar = new QListWidget();
    sidebar->addItem("General");
    sidebar->addItem("Theme & Colors");
    sidebar->setFixedWidth(180);
    sidebar->setSpacing(3);
    QStackedWidget *stackedWidget = new QStackedWidget();
    
    ///general settings
    QWidget *general_frame = new QWidget;
    QHBoxLayout *general_layout = new QHBoxLayout(general_frame);
    QRadioButton *text_editor_rad = new QRadioButton("Text Editor");
    QRadioButton *ide_rad = new QRadioButton("IDE");
    general_layout->addWidget(new QLabel("Choose operating a mode:"));
    general_layout->addWidget(text_editor_rad);
    general_layout->addWidget(ide_rad);
    text_editor_rad->setChecked(true);
    stackedWidget->insertWidget(0 , general_frame);

    ///theme settings
    QWidget *theme_frame = new QWidget;
    QVBoxLayout *theme_layout = new QVBoxLayout(theme_frame);
    QHBoxLayout *theme_choose = new QHBoxLayout();
    QLabel *infoLabel = new QLabel("Choose a theme:");
    QComboBox *themeDropdown = new QComboBox();
    theme_choose->setSpacing(5);
    theme_choose->addWidget(infoLabel);
    theme_choose->addWidget(themeDropdown, 1);
    theme_choose->addStretch();
    themeDropdown->addItems({
        "System Theme", "Dark Contrast", "Dark Gray", "Light", "Solarized"
    });
    themeDropdown->setCurrentText("System Theme");
    theme_layout->addLayout(theme_choose);
    stackedWidget->insertWidget(1, theme_frame);
    QHBoxLayout *radiusLayout = new QHBoxLayout();
    QLabel *radiusLabel = new QLabel("Border radius:");
    QLineEdit *radiusInput = new QLineEdit();
    radiusInput->setPlaceholderText("6");
    radiusInput->setFixedWidth(60);
    QPushButton *applyRadiusBtn = new QPushButton("✅");
    applyRadiusBtn->setToolTip("Apply radius");
    radiusLayout->addWidget(radiusLabel);
    radiusLayout->addWidget(radiusInput);
    radiusLayout->addWidget(applyRadiusBtn);
    theme_layout->addLayout(radiusLayout);
    QObject::connect(applyRadiusBtn, &QPushButton::clicked, [&app, &window, radiusInput]() {
        QString radiusValue = radiusInput->text().isEmpty() ? "6" : radiusInput->text();
        if(radiusValue.toInt() < 0) {
            radiusValue = "0";
        }
        if(radiusValue.toInt() > 16) {
            radiusValue = "16";
            radiusInput->setText("16");
            QMessageBox::information(&window, "Info", "Maximum radius is 16");
        }
        QString styleName = app.styleSheet();        
        app.setStyleSheet(createStyleSheet(currentTheme, radiusValue.toInt()));
    });
    QObject::connect(themeDropdown, &QComboBox::currentTextChanged, [&app, radiusInput](const QString &themeName) {
        radius = radiusInput->text().isEmpty() ? 6 : radiusInput->text().toInt();
        
        if (themeName == "System Theme") {
            applySystemTheme(app, radius);
        } else if (themeName == "Dark Contrast") {
            app.setStyleSheet(createStyleSheet(DarkContrast, radius));
            currentTheme = DarkContrast;
        } else if (themeName == "Dark Gray") {
            app.setStyleSheet(createStyleSheet(DarkGray, radius));
            currentTheme = DarkGray;
        } else if (themeName == "Light") {
            app.setStyleSheet(createStyleSheet(Light, radius));
            currentTheme = Light;
        } else if (themeName == "Solarized") {
            app.setStyleSheet(createStyleSheet(Solarized, radius));
            currentTheme = Solarized;
        }
    });
    QLabel *tba_custom_colors = new QLabel("New custom colors TBA");
    theme_layout->addWidget(tba_custom_colors);
    QPushButton *resetBtn_theme = new QPushButton("🔄");
    resetBtn_theme->setToolTip("Reset theme");
    theme_choose->addWidget(resetBtn_theme);
    QObject::connect(resetBtn_theme, &QPushButton::clicked, [&app]() {
        applySystemTheme(app);
    });
    QPushButton *resetBtn_radius = new QPushButton("🔄");
    resetBtn_radius->setToolTip("Reset radius");
    radiusLayout->addWidget(resetBtn_radius);
    radiusLayout->addStretch();
    QObject::connect(resetBtn_radius, &QPushButton::clicked, [&app, radiusInput]() {
        radiusInput->setText("6");
        app.setStyleSheet(createStyleSheet(currentTheme, 6));
    });

    sidebar->setCurrentRow(0);
    sidebar->setFocusPolicy(Qt::NoFocus);
    QObject::connect(sidebar, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    mainlayout->addWidget(sidebar);
    mainlayout->addWidget(stackedWidget);
    settingsDialog->setLayout(mainlayout);
    settingsDialog->show(); //exec il face sa nu mai poti interactiona cu restul aplicatiei
    /// show il face doar sa apara si practic se termina imediat pentru ca sa terminat functia
    ///si de aia folosesti cu heap allocation ca sa nu se stearga automat
    ///adauga un buton de reset la setari ca sa revina la default
}

/// extensie globala
QMap<QString, QString> extensionMap;
QMap<QString, QPair<QString, QString>> fileTypeFilterMap;

void initializeFileMaps() {
    // umplem extensionMap cu extensii si tipuri de fisiere
    // Text files
    extensionMap["txt"] = "Text Document (.txt)";
    extensionMap["log"] = "Log File (.log)";
    // Web Development
    extensionMap["html"] = "HTML (.html, .htm)";
    extensionMap["htm"] = "HTML (.html, .htm)";
    extensionMap["css"] = "CSS (.css)";
    extensionMap["js"] = "JavaScript (.js)";
    extensionMap["ts"] = "TypeScript (.ts)";
    extensionMap["php"] = "PHP (.php)";
    extensionMap["xml"] = "XML (.xml)";
    extensionMap["json"] = "JSON (.json)";
    // System Programming
    extensionMap["c"] = "C (.c)";
    extensionMap["cpp"] = "C++ (.cpp, .cc, .cxx)";
    extensionMap["cc"] = "C++ (.cpp, .cc, .cxx)";
    extensionMap["cxx"] = "C++ (.cpp, .cc, .cxx)";
    extensionMap["h"] = "C Header (.h)";
    extensionMap["hpp"] = "C++ Header (.hpp, .hxx)";
    extensionMap["hxx"] = "C++ Header (.hpp, .hxx)";
    extensionMap["m"] = "Objective-C (.m)";
    extensionMap["asm"] = "Assembly (.asm, .s)";
    extensionMap["s"] = "Assembly (.asm, .s)";
    extensionMap["rs"] = "Rust (.rs)";
    extensionMap["go"] = "Go (.go)";
    // Scripting
    extensionMap["py"] = "Python (.py)";
    extensionMap["rb"] = "Ruby (.rb)";
    extensionMap["pl"] = "Perl (.pl)";
    extensionMap["lua"] = "Lua (.lua)";
    extensionMap["sh"] = "Shell Script (.sh)";
    extensionMap["bat"] = "Batch File (.bat, .cmd)";
    extensionMap["cmd"] = "Batch File (.bat, .cmd)";
    extensionMap["ps1"] = "PowerShell (.ps1)";
    // JVM Languages
    extensionMap["java"] = "Java (.java)";
    extensionMap["kt"] = "Kotlin (.kt)";
    extensionMap["scala"] = "Scala (.scala)";
    extensionMap["groovy"] = "Groovy (.groovy)";
    // .NET Languages
    extensionMap["cs"] = "C# (.cs)";
    extensionMap["vb"] = "Visual Basic (.vb)";
    extensionMap["fs"] = "F# (.fs)";
    // Data & Config
    extensionMap["yaml"] = "YAML (.yaml, .yml)";
    extensionMap["yml"] = "YAML (.yaml, .yml)";
    extensionMap["toml"] = "TOML (.toml)";
    extensionMap["ini"] = "INI (.ini)";
    extensionMap["csv"] = "CSV (.csv)";
    extensionMap["md"] = "Markdown (.md)";
    extensionMap["pro"] = "QT config file (.pro)";

    fileTypeFilterMap["Normal text file"] = qMakePair(QString("Text Files (*.txt);;All Files (*)"), QString(".txt"));
    
    fileTypeFilterMap["Text Document (.txt)"] = qMakePair(QString("Text Files (*.txt);;All Files (*)"), QString(".txt"));
    fileTypeFilterMap["Log File (.log)"] = qMakePair(QString("Log Files (*.log);;All Files (*)"), QString(".log"));
    
    fileTypeFilterMap["HTML (.html, .htm)"] = qMakePair(QString("HTML Files (*.html *.htm);;All Files (*)"), QString(".html"));
    fileTypeFilterMap["CSS (.css)"] = qMakePair(QString("CSS Files (*.css);;All Files (*)"), QString(".css"));
    fileTypeFilterMap["JavaScript (.js)"] = qMakePair(QString("JavaScript Files (*.js);;All Files (*)"), QString(".js"));
    fileTypeFilterMap["TypeScript (.ts)"] = qMakePair(QString("TypeScript Files (*.ts);;All Files (*)"), QString(".ts"));
    fileTypeFilterMap["PHP (.php)"] = qMakePair(QString("PHP Files (*.php);;All Files (*)"), QString(".php"));
    fileTypeFilterMap["XML (.xml)"] = qMakePair(QString("XML Files (*.xml);;All Files (*)"), QString(".xml"));
    fileTypeFilterMap["JSON (.json)"] = qMakePair(QString("JSON Files (*.json);;All Files (*)"), QString(".json"));
    
    fileTypeFilterMap["C (.c)"] = qMakePair(QString("C Files (*.c);;All Files (*)"), QString(".c"));
    fileTypeFilterMap["C++ (.cpp, .cc, .cxx)"] = qMakePair(QString("C++ Files (*.cpp *.cc *.cxx);;All Files (*)"), QString(".cpp"));
    fileTypeFilterMap["C Header (.h)"] = qMakePair(QString("C Header Files (*.h);;All Files (*)"), QString(".h"));
    fileTypeFilterMap["C++ Header (.hpp, .hxx)"] = qMakePair(QString("C++ Header Files (*.hpp *.hxx);;All Files (*)"), QString(".hpp"));
    fileTypeFilterMap["Objective-C (.m)"] = qMakePair(QString("Objective-C Files (*.m);;All Files (*)"), QString(".m"));
    fileTypeFilterMap["Assembly (.asm, .s)"] = qMakePair(QString("Assembly Files (*.asm *.s);;All Files (*)"), QString(".asm"));
    fileTypeFilterMap["Rust (.rs)"] = qMakePair(QString("Rust Files (*.rs);;All Files (*)"), QString(".rs"));
    fileTypeFilterMap["Go (.go)"] = qMakePair(QString("Go Files (*.go);;All Files (*)"), QString(".go"));
    
    fileTypeFilterMap["Python (.py)"] = qMakePair(QString("Python Files (*.py);;All Files (*)"), QString(".py"));
    fileTypeFilterMap["Ruby (.rb)"] = qMakePair(QString("Ruby Files (*.rb);;All Files (*)"), QString(".rb"));
    fileTypeFilterMap["Perl (.pl)"] = qMakePair(QString("Perl Files (*.pl);;All Files (*)"), QString(".pl"));
    fileTypeFilterMap["Lua (.lua)"] = qMakePair(QString("Lua Files (*.lua);;All Files (*)"), QString(".lua"));
    fileTypeFilterMap["Shell Script (.sh)"] = qMakePair(QString("Shell Script Files (*.sh);;All Files (*)"), QString(".sh"));
    fileTypeFilterMap["Batch File (.bat, .cmd)"] = qMakePair(QString("Batch Files (*.bat *.cmd);;All Files (*)"), QString(".bat"));
    fileTypeFilterMap["PowerShell (.ps1)"] = qMakePair(QString("PowerShell Files (*.ps1);;All Files (*)"), QString(".ps1"));
    
    fileTypeFilterMap["Java (.java)"] = qMakePair(QString("Java Files (*.java);;All Files (*)"), QString(".java"));
    fileTypeFilterMap["Kotlin (.kt)"] = qMakePair(QString("Kotlin Files (*.kt);;All Files (*)"), QString(".kt"));
    fileTypeFilterMap["Scala (.scala)"] = qMakePair(QString("Scala Files (*.scala);;All Files (*)"), QString(".scala"));
    fileTypeFilterMap["Groovy (.groovy)"] = qMakePair(QString("Groovy Files (*.groovy);;All Files (*)"), QString(".groovy"));
    
    fileTypeFilterMap["C# (.cs)"] = qMakePair(QString("C# Files (*.cs);;All Files (*)"), QString(".cs"));
    fileTypeFilterMap["Visual Basic (.vb)"] = qMakePair(QString("Visual Basic Files (*.vb);;All Files (*)"), QString(".vb"));
    fileTypeFilterMap["F# (.fs)"] = qMakePair(QString("F# Files (*.fs);;All Files (*)"), QString(".fs"));
    
    fileTypeFilterMap["YAML (.yaml, .yml)"] = qMakePair(QString("YAML Files (*.yaml *.yml);;All Files (*)"), QString(".yaml"));
    fileTypeFilterMap["TOML (.toml)"] = qMakePair(QString("TOML Files (*.toml);;All Files (*)"), QString(".toml"));
    fileTypeFilterMap["INI (.ini)"] = qMakePair(QString("INI Files (*.ini);;All Files (*)"), QString(".ini"));
    fileTypeFilterMap["CSV (.csv)"] = qMakePair(QString("CSV Files (*.csv);;All Files (*)"), QString(".csv"));
    fileTypeFilterMap["Markdown (.md)"] = qMakePair(QString("Markdown Files (*.md);;All Files (*)"), QString(".md"));
    fileTypeFilterMap["QT config file (.pro)"] = qMakePair(QString("QT Project Files (*.pro);;All Files (*)"), QString(".pro"));
}

QString detectFileType(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();    
    return extensionMap.value(extension, "Normal text file");
}

// custom text edit cu nr liniilor
class LineNumberTextEdit : public QTextEdit {
public:
    LineNumberTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {
        lineNumberArea = new LineNumberArea(this);
        
        connect(document(), &QTextDocument::blockCountChanged, this, &LineNumberTextEdit::updateLineNumberAreaWidth);
        connect(this, &QTextEdit::textChanged, this, &LineNumberTextEdit::updateLineNumberArea);
        connect(this, &QTextEdit::cursorPositionChanged, this, [this]() {
                    updateLineNumberArea();
                    highlightCurrentLine();
                });
        connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &LineNumberTextEdit::updateLineNumberArea);
        updateLineNumberAreaWidth(0);        
        document()->setDocumentMargin(8);
    }
    
    void lineNumberAreaPaintEvent(QPaintEvent *event) {
        QPainter painter(lineNumberArea);
        painter.fillRect(event->rect(), QColor(currentTheme.background).darker(120));        
        QTextBlock block = firstVisibleBlock();
        int blockNumber = block.blockNumber();        
        int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        int bottom = top + qRound(blockBoundingRect(block).height());        
        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);
                painter.setPen(QColor(currentTheme.textColor).lighter(130));                
                painter.drawText(0, top, lineNumberArea->width() - 5, fontMetrics().height(), Qt::AlignRight, number);
            }
            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++blockNumber;
        }
    }
    
    QTextBlock firstVisibleBlock() const {
        // primul bloc vizibil in viewport
        QTextBlock block = document()->begin();
        int scrollY = verticalScrollBar()->value();
        QTextCursor cursor(document());
        
        cursor.movePosition(QTextCursor::Start);
        while (block.isValid()) {
            QRectF blockRect = blockBoundingGeometry(block);
            if (blockRect.top() >= scrollY) {
                return block;
            }
            block = block.next();
        }        
        return document()->begin();
    }
    
    QRectF blockBoundingGeometry(const QTextBlock &block) const {
        return document()->documentLayout()->blockBoundingRect(block);
    }
    QRectF blockBoundingRect(const QTextBlock &block) const {
        return document()->documentLayout()->blockBoundingRect(block);
    }
    QPointF contentOffset() const {
        return QPointF(0, -verticalScrollBar()->value());
    }
    
    int lineNumberAreaWidth() const {
        int digits = 1;
        int max = qMax(1, document()->blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }
        // adaugam un padding de 20px pentru margini
        return 20 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    }
    void forceLineNumberUpdate() {
        QResizeEvent e(size(), size());
        resizeEvent(&e);
        viewport()->update();
        lineNumberArea->update();
    }
    
protected:
    void resizeEvent(QResizeEvent *event) override {
        QTextEdit::resizeEvent(event);        
        QRect cr = contentsRect();
        lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), 
                                        lineNumberAreaWidth(), cr.height()));
    }
    
private:
    class LineNumberArea : public QWidget {
    public:
        LineNumberArea(LineNumberTextEdit *editor) : QWidget(editor), textEdit(editor) {}
        QSize sizeHint() const override {
            return QSize(textEdit->lineNumberAreaWidth(), 0);
        }
        
    protected:
        void paintEvent(QPaintEvent *event) override {
            textEdit->lineNumberAreaPaintEvent(event);
        }
        
    private:
        LineNumberTextEdit *textEdit;
    };
    
    LineNumberArea *lineNumberArea;
    
    void updateLineNumberAreaWidth(int /* newBlockCount */) {
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    }    
    void updateLineNumberArea() {
        updateLineNumberAreaWidth(0);
        lineNumberArea->update();
    }
    void highlightCurrentLine() {
        QList<QTextEdit::ExtraSelection> extraSelections;   
        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;
            QColor lineColor = QColor(currentTheme.buttonHover).lighter(120);
            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }
        setExtraSelections(extraSelections);
    }
};
class FileTabBar : public QTabBar {
public:
    FileTabBar(QWidget *parent = nullptr) : QTabBar(parent) {
        setTabsClosable(true);
        setMovable(true);
        setExpanding(false);
        setUsesScrollButtons(true);
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override {
        int tab = tabAt(event->pos());
        if (tab == -1) {
            // da highlight pe zona goala
            if (onEmptyAreaDoubleClicked) {
                onEmptyAreaDoubleClicked();
            }
        }
        QTabBar::mouseDoubleClickEvent(event);
    }

public:
    std::function<void()> onEmptyAreaDoubleClicked;
};

void saveSession(const QMap<int, QString>& tabToFilePath, FileTabBar* fileTabBar, int currentTabIndex) {
    QSettings settings("PinciuTM", "BasicTextEditor");
    settings.beginWriteArray("openFiles");
    int validFileIndex = 0;
    for (int i = 0; i < fileTabBar->count(); i++) {
        QString filePath = tabToFilePath.value(i, "");
        if (filePath.isEmpty()) {
            continue;
        }        
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists() && fileInfo.isFile()) {
            settings.setArrayIndex(validFileIndex);
            settings.setValue("filePath", filePath);
            settings.setValue("wasActive", i == currentTabIndex);
            validFileIndex++;
        }
    }
    settings.endArray();
    settings.setValue("sessionCount", validFileIndex);
}
void restoreSession(QMainWindow& window, FileTabBar* fileTabBar, QStackedWidget* textEditStack,
                   QMap<int, QString>& tabToFilePath, QMap<int, bool>& tabModified,
                   QPushButton* file_type, QLabel* char_line_label, 
                   std::function<void(LineNumberTextEdit*)> connectEditorSignals) {
    QSettings settings("PinciuTM", "BasicTextEditor");   
    int sessionCount = settings.value("sessionCount", 0).toInt();
    if (sessionCount == 0) return;
    int activeTabIndex = -1;
    bool hasValidFiles = false;
    int size = settings.beginReadArray("openFiles");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QString filePath = settings.value("filePath").toString();
        bool wasActive = settings.value("wasActive", false).toBool();
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            continue;
        }
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }
        QByteArray rawData = file.readAll();
        QString content = QString::fromUtf8(rawData);
        file.close();
        LineNumberTextEdit *newEditor = new LineNumberTextEdit();
        textEditStack->addWidget(newEditor);
        QFileInfo fileInfo2(filePath);
        QString baseName = fileInfo2.fileName();        
        int newTab = fileTabBar->addTab(baseName + " 🔵");
        tabToFilePath[newTab] = filePath;
        tabModified[newTab] = false;
        newEditor->setPlainText(content);
        newEditor->forceLineNumberUpdate();
        
        // semnale de conectare pentru editor
        QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
            if (!tabModified[newTab]) {
                tabModified[newTab] = true;
                QString tabText = fileTabBar->tabText(newTab);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(newTab, tabText);
            }
        });
        connectEditorSignals(newEditor);
        if (wasActive) {
            activeTabIndex = newTab;
        }
        hasValidFiles = true;
    }
    settings.endArray();
    if (hasValidFiles && fileTabBar->count() > sessionCount) {
        QWidget *firstWidget = textEditStack->widget(0);
        if (firstWidget) {
            textEditStack->removeWidget(firstWidget);
            delete firstWidget;
            fileTabBar->removeTab(0);
            QMap<int, QString> newTabToFilePath;
            QMap<int, bool> newTabModified;
            for (int i = 0; i < fileTabBar->count(); i++) {
                newTabToFilePath[i] = tabToFilePath[i + 1];
                newTabModified[i] = tabModified[i + 1];
            }
            tabToFilePath = newTabToFilePath;
            tabModified = newTabModified;            
            if (activeTabIndex > 0) activeTabIndex--;
        }
    }
    // seteaza tab-ul activ
    if (activeTabIndex >= 0 && activeTabIndex < fileTabBar->count()) {
        fileTabBar->setCurrentIndex(activeTabIndex);
        textEditStack->setCurrentIndex(activeTabIndex);
        QString filePath = tabToFilePath[activeTabIndex];
        file_type->setText(detectFileType(filePath));
        if (ostype == 1) {
            filePath.replace("/", "\\");
        }
        window.setWindowTitle(filePath + " - Notepad+");
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            QString content = currentEditor->toPlainText();
            QByteArray rawData = content.toUtf8();
            auto [lineCount, characterCount] = text_lines_characters(rawData);
            char_line_label->setText(QString("length: %1  lines: %2").arg(characterCount).arg(lineCount));
        }
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMap<int, QString> tabToFilePath;
    QMap<int, bool> tabModified;
    initializeFileMaps();
    QMainWindow window;
    window.setAttribute(Qt::WA_QuitOnClose, false);
    window.setWindowIcon(QIcon(":/icon.png"));
    window.setWindowTitle("Notepad+");

    QMenuBar *upper_menuBar = new QMenuBar(&window);
    window.setMenuBar(upper_menuBar);    
    // File menu
    QMenu *upper_fileMenu = upper_menuBar->addMenu("File");
    QAction *upper_newAction = new QAction("New", &window);
    QAction *upper_openAction = new QAction("Open", &window);
    QAction *upper_saveAction = new QAction("Save", &window);
    QAction *upper_saveAsAction = new QAction("Save As", &window);
    QAction *upper_saveAllAction = new QAction("Save All", &window);
    QAction *upper_closeAction = new QAction("Close", &window);
    QAction *upper_closeAllAction = new QAction("Close All", &window);
    QAction *upper_exitAction = new QAction("Exit", &window);
    // shortcuts
    upper_newAction->setShortcut(QKeySequence("Ctrl+N"));
    upper_openAction->setShortcut(QKeySequence("Ctrl+O"));
    upper_saveAction->setShortcut(QKeySequence("Ctrl+S"));
    upper_saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    upper_closeAction->setShortcut(QKeySequence("Ctrl+W"));
    upper_closeAllAction->setShortcut(QKeySequence("Ctrl+Shift+W"));
    upper_exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    // Add actions to file menu
    upper_fileMenu->addAction(upper_newAction);
    upper_fileMenu->addAction(upper_openAction);
    upper_fileMenu->addAction(upper_saveAction);
    upper_fileMenu->addAction(upper_saveAsAction);
    upper_fileMenu->addAction(upper_saveAllAction);
    upper_fileMenu->addAction(upper_closeAction);
    upper_fileMenu->addAction(upper_closeAllAction);
    upper_fileMenu->addSeparator();
    upper_fileMenu->addAction(upper_exitAction);
    // Settings menu
    QMenu *upper_settingsMenu = upper_menuBar->addMenu("Settings");
    QAction *upper_settingsAction = new QAction("Settings", &window);
    upper_settingsMenu->addAction(upper_settingsAction);
    // Help menu
    QMenu *upper_helpMenu = upper_menuBar->addMenu("Help");
    QAction *upper_helpAction = new QAction("Help", &window);
    upper_helpMenu->addAction(upper_helpAction);
    QString currentFilePath = "";
    LineNumberTextEdit* firstEditor = new LineNumberTextEdit();
    firstEditor->setPlaceholderText("Start writting !");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    window.setAutoFillBackground(true);
    app.setPalette(darkPalette);
    QVBoxLayout mainlayout;
    
    QToolBar *toolbar = new QToolBar(&window);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolbar->setMovable(false);
    window.addToolBar(Qt::TopToolBarArea, toolbar);
    
    QPushButton newButton("📄");
    QPushButton loadButton("📁"); ///open button
    QPushButton saveButton("💾");
    QPushButton saveAllButton;
    QPushButton closeButton;
    QPushButton closeAllButton;
    QPushButton printButton("🖨️");
    QPushButton cutButton("✂️");
    QPushButton copyButton;
    QPushButton pasteButton("📋");
    QPushButton undoButton("↩️");
    QPushButton redoButton("↪️");
    QPushButton findButton("🔍");
    QPushButton replaceButton("🔁");
    QPushButton settingsButton("⚙️");

    newButton.setToolTip("New File");
    loadButton.setToolTip("Open File");
    saveButton.setToolTip("Save File");
    saveAllButton.setToolTip("Save All Files");
    closeButton.setToolTip("Close File");
    closeAllButton.setToolTip("Close All Files");
    printButton.setToolTip("Print File");
    cutButton.setToolTip("Cut");
    copyButton.setToolTip("Copy");
    pasteButton.setToolTip("Paste");
    undoButton.setToolTip("Undo");
    redoButton.setToolTip("Redo");
    findButton.setToolTip("Find");
    replaceButton.setToolTip("Replace");    
    settingsButton.setToolTip("Settings");
    
    newButton.setCursor(Qt::PointingHandCursor);
    loadButton.setCursor(Qt::PointingHandCursor);
    saveButton.setCursor(Qt::PointingHandCursor);
    saveAllButton.setCursor(Qt::PointingHandCursor);
    closeButton.setCursor(Qt::PointingHandCursor);
    closeAllButton.setCursor(Qt::PointingHandCursor);
    printButton.setCursor(Qt::PointingHandCursor);
    cutButton.setCursor(Qt::PointingHandCursor);
    copyButton.setCursor(Qt::PointingHandCursor);
    pasteButton.setCursor(Qt::PointingHandCursor);
    undoButton.setCursor(Qt::PointingHandCursor);
    redoButton.setCursor(Qt::PointingHandCursor);
    findButton.setCursor(Qt::PointingHandCursor);
    replaceButton.setCursor(Qt::PointingHandCursor);
    settingsButton.setCursor(Qt::PointingHandCursor);

    newButton.setFlat(true);
    loadButton.setFlat(true);
    saveButton.setFlat(true);
    saveAllButton.setFlat(true);
    closeButton.setFlat(true);
    closeAllButton.setFlat(true);
    printButton.setFlat(true);
    cutButton.setFlat(true);
    copyButton.setFlat(true);
    pasteButton.setFlat(true);
    undoButton.setFlat(true);
    redoButton.setFlat(true);
    findButton.setFlat(true);
    replaceButton.setFlat(true);
    settingsButton.setFlat(true);

    saveAllButton.setIcon(QIcon(":/save_all.png"));
    closeButton.setIcon(QIcon(":/close_file.png"));
    closeAllButton.setIcon(QIcon(":/close_all.png"));
    copyButton.setIcon(QIcon(":/copy.png"));
    QSize iconSize(16, 16);
    saveAllButton.setIconSize(iconSize);
    closeButton.setIconSize(iconSize);
    closeAllButton.setIconSize(iconSize);
    copyButton.setIconSize(iconSize);
    
    toolbar->addWidget(&newButton);
    toolbar->addWidget(&loadButton);
    toolbar->addWidget(&saveButton);
    toolbar->addWidget(&saveAllButton);
    toolbar->addSeparator();
    toolbar->addWidget(&closeButton);
    toolbar->addWidget(&closeAllButton);
    toolbar->addSeparator();
    toolbar->addWidget(&printButton);
    toolbar->addSeparator();
    toolbar->addWidget(&cutButton);
    toolbar->addWidget(&copyButton);
    toolbar->addWidget(&pasteButton);
    toolbar->addSeparator();
    toolbar->addWidget(&undoButton);
    toolbar->addWidget(&redoButton);
    toolbar->addSeparator();
    toolbar->addWidget(&findButton);
    toolbar->addWidget(&replaceButton);
    toolbar->addSeparator();
    toolbar->addWidget(&settingsButton);
        
    currentTheme = applySystemTheme(app);
    FileTabBar *fileTabBar = new FileTabBar();
    QStackedWidget *textEditStack = new QStackedWidget();
    textEditStack->addWidget(firstEditor);
    int firstTab = fileTabBar->addTab("New File 🔵");
    tabToFilePath[firstTab] = "";
    tabModified[firstTab] = false;
    QObject::connect(firstEditor, &QTextEdit::textChanged, [fileTabBar, firstTab, &tabModified]() {
        if (!tabModified[firstTab]) {
            tabModified[firstTab] = true;
            QString tabText = fileTabBar->tabText(firstTab);
            tabText.replace("🔵", "🔴");
            fileTabBar->setTabText(firstTab, tabText);
        }
    });
    
    QLabel *char_line_label = new QLabel("Ln: 0  Col: 1");
    QLabel *positionLabel = new QLabel("Ln: 1  Col: 1 Pos: 1");
    QLabel *encodingLabel = new QLabel("UTF-8");
    QPushButton *eolButton = new QPushButton("Windows (CRLF)");    eolButton->setFlat(true);
    eolButton->setCursor(Qt::PointingHandCursor);

    QMenu *eolMenu = new QMenu(eolButton);
    // functie ca sa conecteze cursorul si textul schimbat la editor
    auto connectEditorSignals = [positionLabel, char_line_label](LineNumberTextEdit* editor) {
        QObject::connect(editor, &QTextEdit::cursorPositionChanged, [positionLabel, editor]() {
            QTextCursor cursor = editor->textCursor();
            int line = cursor.blockNumber() + 1;
            int column = cursor.columnNumber() + 1;
            QString text = editor->toPlainText().left(cursor.position());
            int newlineCount = text.count('\n');
            int position = cursor.position() + newlineCount + 1;            
            positionLabel->setText(QString("Ln: %1  Col: %2 Pos: %3").arg(line).arg(column).arg(position));
        });
        QObject::connect(editor, &QTextEdit::textChanged, [char_line_label, editor]() {
            QString content = editor->toPlainText();
            QByteArray rawData = content.toUtf8();
            auto [lineCount, characterCount] = text_lines_characters(rawData);
            char_line_label->setText(QString("length: %1  lines: %2").arg(characterCount).arg(lineCount));
        });
    };
    connectEditorSignals(firstEditor);

    fileTabBar->onEmptyAreaDoubleClicked = [fileTabBar, textEditStack, &tabToFilePath, &tabModified, connectEditorSignals]() {
        LineNumberTextEdit *newEditor = new LineNumberTextEdit();
        newEditor->setPlaceholderText("Start writing!");
        textEditStack->addWidget(newEditor);
        int newTab = fileTabBar->addTab("New File 🔵");
        tabToFilePath[newTab] = "";
        tabModified[newTab] = false;
        fileTabBar->setCurrentIndex(newTab);
        textEditStack->setCurrentIndex(newTab);        
        // conecteaza semnalele pentru noul editor
        QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
            if (!tabModified[newTab]) {
                tabModified[newTab] = true;
                QString tabText = fileTabBar->tabText(newTab);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(newTab, tabText);
            }
        });
        connectEditorSignals(newEditor);
    };
    QStatusBar *statusBar = new QStatusBar(&window);
    window.setStatusBar(statusBar);
    QWidget *spacer1 = new QWidget();
    QWidget *spacer2 = new QWidget();
    QWidget *spacer3 = new QWidget();
    QWidget *spacer4 = new QWidget();
    spacer1->setFixedWidth(10);
    spacer2->setFixedWidth(10);
    spacer3->setFixedWidth(10);
    spacer4->setFixedWidth(10);
    QPushButton *file_type = new QPushButton("Normal text file");
    file_type->setFlat(true);
    file_type->setCursor(Qt::PointingHandCursor);
    QMenu *fileTypeMenu = new QMenu(file_type);
    QAction *normalTextAction = fileTypeMenu->addAction("Normal text file");
    fileTypeMenu->addSeparator();
    QObject::connect(normalTextAction, &QAction::triggered, [file_type]() {
        file_type->setText("Normal text file");
    });
    ///web dev
    QMenu *webDevMenu = fileTypeMenu->addMenu("Web Development");
    webDevMenu->addAction("HTML (.html, .htm)");
    webDevMenu->addAction("CSS (.css)");
    webDevMenu->addAction("JavaScript (.js)");
    webDevMenu->addAction("TypeScript (.ts)");
    webDevMenu->addAction("PHP (.php)");
    webDevMenu->addAction("XML (.xml)");
    webDevMenu->addAction("JSON (.json)");
    ///limbaje de programare
    QMenu *sysProgMenu = fileTypeMenu->addMenu("System Programming");
    sysProgMenu->addAction("C (.c)");
    sysProgMenu->addAction("C++ (.cpp, .cc, .cxx)");
    sysProgMenu->addAction("C Header (.h)");
    sysProgMenu->addAction("C++ Header (.hpp, .hxx)");
    sysProgMenu->addAction("Objective-C (.m)");
    sysProgMenu->addAction("Assembly (.asm, .s)");
    sysProgMenu->addAction("Rust (.rs)");
    sysProgMenu->addAction("Go (.go)");
    ///limbaje de scripting
    QMenu *scriptMenu = fileTypeMenu->addMenu("Scripting");
    scriptMenu->addAction("Python (.py)");
    scriptMenu->addAction("Ruby (.rb)");
    scriptMenu->addAction("Perl (.pl)");
    scriptMenu->addAction("Lua (.lua)");
    scriptMenu->addAction("Shell Script (.sh)");
    scriptMenu->addAction("Batch File (.bat, .cmd)");
    scriptMenu->addAction("PowerShell (.ps1)");
    ///jvm
    QMenu *jvmMenu = fileTypeMenu->addMenu("JVM Languages");
    jvmMenu->addAction("Java (.java)");
    jvmMenu->addAction("Kotlin (.kt)");
    jvmMenu->addAction("Scala (.scala)");
    jvmMenu->addAction("Groovy (.groovy)");
    ///limbaje .NET
    QMenu *dotNetMenu = fileTypeMenu->addMenu("Microsoft .NET");
    dotNetMenu->addAction("C# (.cs)");
    dotNetMenu->addAction("Visual Basic (.vb)");
    dotNetMenu->addAction("F# (.fs)");
    ///config files
    QMenu *dataMenu = fileTypeMenu->addMenu("Data & Config");
    dataMenu->addAction("YAML (.yaml, .yml)");
    dataMenu->addAction("TOML (.toml)");
    dataMenu->addAction("INI (.ini)");
    dataMenu->addAction("CSV (.csv)");
    dataMenu->addAction("Markdown (.md)");
    dataMenu->addAction("QT config file (.pro)");
    file_type->setMenu(fileTypeMenu);
    for (QAction* action : fileTypeMenu->actions()) {
        if (!action->menu() && !action->isSeparator()) {
            QObject::connect(action, &QAction::triggered, [file_type, action]() {
                file_type->setText(action->text());
            });
        }
    }
    QList<QMenu*> submenus = fileTypeMenu->findChildren<QMenu*>();
    for (QMenu* submenu : submenus) {
        for (QAction* action : submenu->actions()) {
            QObject::connect(action, &QAction::triggered, [file_type, action]() {
                file_type->setText(action->text());
            });
        }
    }

    eolMenu->addAction("Windows (CRLF)");
    eolMenu->addAction("Unix (LF)");
    eolMenu->addAction("Mac (CR)");
    QFontMetrics fontMetrics(eolButton->font());
    int width = fontMetrics.horizontalAdvance("Windows (CRLF)") + 20;
    eolButton->setFixedWidth(width);
    QPushButton *cursor_caret = new QPushButton("INS");
    cursor_caret->setFlat(true);
    cursor_caret->setCursor(Qt::PointingHandCursor); /// adauga chestia asta cu mana si la restul butoanelor din program
    if(ostype == 1) {
        eolButton->setText("Windows (CRLF)");
        eolButton->setToolTip("Windows (CRLF)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    }
    else if(ostype == 2) {
        eolButton->setText("Mac (CR)");
        eolButton->setToolTip("Mac (CR)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    }
    else if(ostype == 3) {
        eolButton->setText("Unix (LF)");
        eolButton->setToolTip("Unix (LF)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    }
    connectEditorSignals(firstEditor);

    QObject::connect(fileTabBar, &QTabBar::currentChanged, [textEditStack, &tabToFilePath, file_type, &window, positionLabel, char_line_label](int index) {
        if (index >= 0) {
            textEditStack->setCurrentIndex(index);
            QString filePath = tabToFilePath[index];
            if (!filePath.isEmpty()) {
                file_type->setText(detectFileType(filePath));
                if (ostype == 1) {
                    filePath.replace("/", "\\");
                }
                window.setWindowTitle(filePath + " - Notepad+");
            } else {
                file_type->setText("Normal text file");
                window.setWindowTitle("Notepad+");
            }
            
            ///updateaza pozitia cursorului
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (currentEditor) {
                QTextCursor cursor = currentEditor->textCursor();
                int line = cursor.blockNumber() + 1;
                int column = cursor.columnNumber() + 1;
                QString text = currentEditor->toPlainText().left(cursor.position());
                int newlineCount = text.count('\n');
                int position = cursor.position() + newlineCount + 1;                
                positionLabel->setText(QString("Ln: %1  Col: %2 Pos: %3").arg(line).arg(column).arg(position));
                ///updateaza stats pt tab curent
                QString content = currentEditor->toPlainText();
                QByteArray rawData = content.toUtf8();
                auto [lineCount, characterCount] = text_lines_characters(rawData);
                char_line_label->setText(QString("length: %1  lines: %2").arg(characterCount).arg(lineCount));
            }
        }
    });
    QObject::connect(fileTabBar, &QTabBar::tabCloseRequested, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, file_type, &saveButton](int index) {
        if (tabModified[index]) {
            QMessageBox msgBox(&window);
            msgBox.setText("The document has been modified.");
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            
            switch (ret) {
                case QMessageBox::Save:
                    saveButton.click();
                    break;
                case QMessageBox::Cancel:
                    return;
                default:
                    break;
            }
        }
        int currentIndex = fileTabBar->currentIndex();        
        QWidget *widget = textEditStack->widget(index);
        textEditStack->removeWidget(widget);
        delete widget;        
        fileTabBar->removeTab(index);
    
        // Update maps
        for (int i = index; i < fileTabBar->count(); i++) {
            tabToFilePath[i] = tabToFilePath[i+1];
            tabModified[i] = tabModified[i+1];
        }
        tabToFilePath.remove(fileTabBar->count());
        tabModified.remove(fileTabBar->count());
        
        if (fileTabBar->count() == 0) {
            LineNumberTextEdit *newEditor = new LineNumberTextEdit();
            newEditor->setPlaceholderText("Start writing!");
            textEditStack->addWidget(newEditor);
            int newTab = fileTabBar->addTab("New File 🔵");
            tabToFilePath[newTab] = "";
            tabModified[newTab] = false;
            QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
                if (!tabModified[newTab]) {
                    tabModified[newTab] = true;
                    QString tabText = fileTabBar->tabText(newTab);
                    tabText.replace("🔵", "🔴");
                    fileTabBar->setTabText(newTab, tabText);
                }
            });            
            fileTabBar->setCurrentIndex(newTab);
        } else if (currentIndex == index) {
            int newIndex = (index < fileTabBar->count()) ? index : fileTabBar->count() - 1; ///daca ai inchis tab curent , il set main pe precedent
            fileTabBar->setCurrentIndex(newIndex);
        }
        int newCurrentIndex = fileTabBar->currentIndex();
        if (newCurrentIndex >= 0) {
            QString filePath = tabToFilePath[newCurrentIndex];
            if (filePath.isEmpty()) {
                window.setWindowTitle("Notepad+");
                file_type->setText("Normal text file");
            } else {
                if (ostype == 1) {
                    filePath.replace("/", "\\");
                }
                window.setWindowTitle(filePath + " - Notepad+");
                file_type->setText(detectFileType(filePath));
            }
        }
    });
    statusBar->addWidget(file_type);
    statusBar->addPermanentWidget(char_line_label);
    statusBar->addPermanentWidget(spacer1);
    statusBar->addPermanentWidget(positionLabel);
    statusBar->addPermanentWidget(spacer2);
    statusBar->addPermanentWidget(encodingLabel);
    statusBar->addPermanentWidget(spacer3);
    statusBar->addPermanentWidget(eolButton);
    statusBar->addPermanentWidget(spacer4);
    statusBar->addPermanentWidget(cursor_caret);
    QObject::connect(firstEditor, &QTextEdit::cursorPositionChanged, [positionLabel, firstEditor]() {
        QTextCursor cursor = firstEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        QString text = firstEditor->toPlainText().left(cursor.position());
        int newlineCount = text.count('\n');
        int position = cursor.position() + newlineCount + 1;        
        positionLabel->setText(QString("Ln: %1  Col: %2 Pos: %3").arg(line).arg(column).arg(position));
    });
    QObject::connect(cursor_caret, &QPushButton::clicked, [cursor_caret, firstEditor]() {
        bool newMode = !firstEditor->overwriteMode();///cursor caret - insert si overwrite mode
        firstEditor->setOverwriteMode(newMode);
        cursor_caret->setText(newMode ? "OVR" : "INS");
    });
    QObject::connect(eolButton, &QPushButton::clicked, [eolButton, eolMenu]() {
        eolMenu->exec(eolButton->mapToGlobal(QPoint(0, -eolMenu->sizeHint().height())));
    });
    QObject::connect(eolMenu->actions().at(0), &QAction::triggered, [eolButton, firstEditor]() {
        eolButton->setText("Windows (CRLF)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    });
    QObject::connect(eolMenu->actions().at(1), &QAction::triggered, [eolButton, firstEditor]() {
        eolButton->setText("Unix (LF)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    });
    QObject::connect(eolMenu->actions().at(2), &QAction::triggered, [eolButton, firstEditor]() {
        eolButton->setText("Mac (CR)");
        QString content = firstEditor->toPlainText();
        content.replace("\r\n", "\n").replace("\n", "\r");
    });
    QObject::connect(&saveButton, &QPushButton::clicked, 
        [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, file_type, &currentFilePath]() {
            /// salv fișierul curent
            int currentIndex = fileTabBar->currentIndex();
            if (currentIndex < 0) return;
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (!currentEditor) return;
            QString filePath = tabToFilePath[currentIndex];
            if (filePath.isEmpty()) {
                QString currentType = file_type->text();
                QString filter = "All Files (*)";
                QString defaultExt = "";
                if (fileTypeFilterMap.contains(currentType)) {
                    filter = fileTypeFilterMap[currentType].first;
                    defaultExt = fileTypeFilterMap[currentType].second;
                }                
                filePath = QFileDialog::getSaveFileName(&window, "Save as", "C:/", filter);
                if (filePath.isEmpty()) return;
                if (!filePath.contains(".") && !defaultExt.isEmpty()) {
                    filePath += defaultExt;
                }
            }            
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QString content = currentEditor->toPlainText();
                QTextStream out(&file);
                out << content;
                file.close();
                tabToFilePath[currentIndex] = filePath;
                tabModified[currentIndex] = false;
                QFileInfo fileInfo(filePath);
                QString baseName = fileInfo.fileName();
                fileTabBar->setTabText(currentIndex, baseName + " 🔵");
                currentFilePath = filePath;
                if (ostype == 1) {
                    currentFilePath.replace("/", "\\");
                }
                window.setWindowTitle(currentFilePath + " - Notepad+");
            } else {
                QMessageBox::warning(&window, "Error", "Could not write to file.");
            }
        });
    QObject::connect(&loadButton, &QPushButton::clicked, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, &currentFilePath, file_type, char_line_label, connectEditorSignals]() {
        QString fileName = QFileDialog::getOpenFileName(&window, "Open File", "C:/", 
            "All Files (*);;C++ files (*.cpp);;Text Files (*.txt)");
        if (fileName.isEmpty()) return;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QByteArray rawData = file.readAll();
            auto [lineCount, characterCount] = text_lines_characters(rawData);
            QString content = QString::fromUtf8(rawData);
            file.close();
            LineNumberTextEdit *newEditor = new LineNumberTextEdit();
            textEditStack->addWidget(newEditor);
            QFileInfo fileInfo(fileName);
            QString baseName = fileInfo.fileName();            
            int newTab = fileTabBar->addTab(baseName + " 🔵");
            tabToFilePath[newTab] = fileName;
            tabModified[newTab] = false;
            newEditor->setPlainText(content);
            newEditor->forceLineNumberUpdate();
            QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
                if (!tabModified[newTab]) {
                    tabModified[newTab] = true;
                    QString tabText = fileTabBar->tabText(newTab);
                    tabText.replace("🔵", "🔴");
                    fileTabBar->setTabText(newTab, tabText);
                }
            });
            connectEditorSignals(newEditor);
            fileTabBar->setCurrentIndex(newTab);
            textEditStack->setCurrentIndex(newTab);
            currentFilePath = fileName;
            if (ostype == 1) {
                currentFilePath.replace("/", "\\");
            }
            window.setWindowTitle(currentFilePath + " - Notepad+");
            char_line_label->setText(QString("length: %1  lines: %2").arg(characterCount).arg(lineCount));
            file_type->setText(detectFileType(fileName));
        } else {
            QMessageBox::warning(&window, "Error", "Could not open file for reading.");
        }
    });
    QObject::connect(&closeButton, &QPushButton::clicked, [fileTabBar]() {
        int currentIndex = fileTabBar->currentIndex();
        if (currentIndex < 0) return;
        fileTabBar->tabCloseRequested(currentIndex);
    });
    QObject::connect(&closeAllButton, &QPushButton::clicked, [&window, fileTabBar, file_type]() {
        for(int i = 0 ; i < fileTabBar->count();  i++) {
            fileTabBar->tabCloseRequested(0);
        }
        window.setWindowTitle("Notepad+");
        file_type->setText("Normal text file");
    });
    QObject::connect(&newButton, &QPushButton::clicked, [&fileTabBar, textEditStack, &tabToFilePath, &tabModified, connectEditorSignals]() {
        LineNumberTextEdit *newEditor = new LineNumberTextEdit();
        newEditor->setPlaceholderText("Start writing!");
        textEditStack->addWidget(newEditor);
        int newTab = fileTabBar->addTab("New File 🔵");
        tabToFilePath[newTab] = "";
        tabModified[newTab] = false;
        fileTabBar->setCurrentIndex(newTab);
        textEditStack->setCurrentIndex(newTab);
        QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
            if (!tabModified[newTab]) {
                tabModified[newTab] = true;
                QString tabText = fileTabBar->tabText(newTab);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(newTab, tabText);
            }
        });
        connectEditorSignals(newEditor);
    });
    QObject::connect(&saveAllButton, &QPushButton::clicked, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, file_type, &currentFilePath]() {
        for (int i = 0; i < fileTabBar->count(); i++) {
            LineNumberTextEdit *editor = static_cast<LineNumberTextEdit*>(textEditStack->widget(i));
            if (!editor) continue;            
            QString filePath = tabToFilePath[i];
            if (filePath.isEmpty()) {
                QString content = editor->toPlainText().trimmed();
                if (content.isEmpty()) {
                    continue; // da skip la fisierele goale
                }
                // ia extensia si tipul de fisier curent din status bar
                QString currentType = file_type->text();
                QString filter = "All Files (*)";
                QString defaultExt = "";
                if (fileTypeFilterMap.contains(currentType)) {
                    filter = fileTypeFilterMap[currentType].first;
                    defaultExt = fileTypeFilterMap[currentType].second;
                }                
                filePath = QFileDialog::getSaveFileName(&window, "Save as", "C:/", filter);
                if (filePath.isEmpty()) continue;
                if (!filePath.contains(".") && !defaultExt.isEmpty()) {
                    filePath += defaultExt;
                }
            }            
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QString content = editor->toPlainText();                
                QTextStream out(&file);
                out << content;
                file.close();
                tabToFilePath[i] = filePath;
                tabModified[i] = false;
                QFileInfo fileInfo(filePath);
                QString baseName = fileInfo.fileName();
                fileTabBar->setTabText(i, baseName + " 🔵");
                currentFilePath = filePath;
                if (ostype == 1) {
                    currentFilePath.replace("/", "\\");
                }
                window.setWindowTitle(currentFilePath + " - Notepad+");
            } else {
                QMessageBox::warning(&window, "Error", "Could not write to file: " + filePath);
            }
        }
    });
    QObject::connect(&printButton, &QPushButton::clicked, [&window, textEditStack]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            QPrinter printer;
            QPrintDialog printDialog(&printer, &window);
            if (printDialog.exec() == QDialog::Accepted) {
                currentEditor->print(&printer);
            }
        } else {
            QMessageBox::warning(&window, "Error", "No editor is currently active.");
        }
    });
    QObject::connect(&cutButton, &QPushButton::clicked, [&]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            currentEditor->cut();
        }
    });
    QObject::connect(&copyButton, &QPushButton::clicked, [&]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            currentEditor->copy();
        }
    });
    QObject::connect(&pasteButton, &QPushButton::clicked, [&]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            currentEditor->paste();
        }
    });
    QObject::connect(&undoButton, &QPushButton::clicked, [&]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            currentEditor->undo();
        }
    });
    QObject::connect(&redoButton, &QPushButton::clicked, [&]() {
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (currentEditor) {
            currentEditor->redo();
        }
    });

    auto createFindReplcaeDialog = [](QMainWindow& window, QStackedWidget* textEditStack){
        QDialog *find_replace = new QDialog(&window);
        find_replace->setAttribute(Qt::WA_DeleteOnClose);
        find_replace->setFixedSize(500, 300);
        find_replace->setWindowTitle("Find/Replace");
        QVBoxLayout *findReplace_mainlayout = new QVBoxLayout(find_replace);
        QListWidget *upper_functions = new QListWidget(find_replace);
        upper_functions->addItem("Find");
        upper_functions->addItem("Replace");
        upper_functions->setCurrentRow(0);
        upper_functions->setFlow(QListView::LeftToRight);
        upper_functions->setFixedHeight(50);
        upper_functions->setMaximumWidth(200);

        QStackedWidget *findReplace_stack = new QStackedWidget(find_replace);
        QWidget *findWidget = new QWidget(find_replace);
        QHBoxLayout *findMainLayout = new QHBoxLayout(findWidget);
        
        QHBoxLayout *findInputLayout = new QHBoxLayout();
        QLabel *find_input = new QLabel("Find:", findWidget);
        find_input->setMinimumWidth(80);
        QLineEdit *findLineEdit = new QLineEdit(findWidget);
        findLineEdit->setPlaceholderText("Enter text to find...");        
        findInputLayout->addWidget(find_input);
        findInputLayout->addWidget(findLineEdit);
        findMainLayout->addLayout(findInputLayout);
        
        QVBoxLayout *findButtonLayout = new QVBoxLayout();
        findButtonLayout->addSpacing(70);
        QPushButton *findNextButton = new QPushButton("Find Next", findWidget);
        QPushButton *findPrevButton = new QPushButton("Find Previous", findWidget);
        findNextButton->setMaximumWidth(100);
        findPrevButton->setMaximumWidth(100);
        findButtonLayout->addWidget(findNextButton);
        findButtonLayout->addSpacing(10);
        findButtonLayout->addWidget(findPrevButton);
        findButtonLayout->addStretch();
        
        findMainLayout->addLayout(findButtonLayout);
        findReplace_stack->addWidget(findWidget);

        QWidget *replaceWidget = new QWidget(find_replace);
        QVBoxLayout *replaceMainLayout = new QVBoxLayout(replaceWidget);
        QHBoxLayout *replaceFindLayout = new QHBoxLayout();
        QLabel *replace_input = new QLabel("Find:", replaceWidget);
        replace_input->setMinimumWidth(80);
        QLineEdit *replaceFindLineEdit = new QLineEdit(replaceWidget);
        replaceFindLineEdit->setPlaceholderText("Enter text to find...");
        replaceFindLayout->addWidget(replace_input);
        replaceFindLayout->addWidget(replaceFindLineEdit);
        replaceMainLayout->addLayout(replaceFindLayout);

        QHBoxLayout *replaceWithLayout = new QHBoxLayout();
        QLabel *replaceWithLabel = new QLabel("Replace with:", replaceWidget);
        replaceWithLabel->setMinimumWidth(80);
        QLineEdit *replaceWithLineEdit = new QLineEdit(replaceWidget);
        replaceWithLineEdit->setPlaceholderText("Enter text to replace with...");
        replaceWithLayout->addWidget(replaceWithLabel);
        replaceWithLayout->addWidget(replaceWithLineEdit);
        replaceMainLayout->addLayout(replaceWithLayout);

        QPushButton *replaceNextButton = new QPushButton("Replace Next", replaceWidget);
        QPushButton *replaceAllButton = new QPushButton("Replace All", replaceWidget);
        replaceNextButton->setMaximumWidth(100);
        replaceAllButton->setMaximumWidth(100);
        replaceFindLayout->addWidget(replaceNextButton);
        replaceWithLayout->addWidget(replaceAllButton);
        
        findReplace_stack->addWidget(replaceWidget);        
        findReplace_mainlayout->addWidget(upper_functions);
        findReplace_mainlayout->addWidget(findReplace_stack);
        
        QObject::connect(upper_functions, &QListWidget::currentRowChanged, findReplace_stack, &QStackedWidget::setCurrentIndex);
        
        QObject::connect(findNextButton, &QPushButton::clicked, [&]() {
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (currentEditor) {
                QString searchText = findLineEdit->text();
                if (!searchText.isEmpty()) {
                    QTextDocument *doc = currentEditor->document();
                    QTextCursor cursor = doc->find(searchText, currentEditor->textCursor());
                    if (!cursor.isNull()) {
                        currentEditor->setTextCursor(cursor);
                    } else {
                        QMessageBox::information(&window, "Find", "Text not found.");
                    }
                }
            }
        });
        QObject::connect(findPrevButton, &QPushButton::clicked, [&]() {
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (currentEditor) {
                QString searchText = findLineEdit->text();
                if (!searchText.isEmpty()) {
                    QTextDocument *doc = currentEditor->document();
                    QTextCursor cursor = doc->find(searchText, currentEditor->textCursor(), QTextDocument::FindBackward);
                    if (!cursor.isNull()) {
                        currentEditor->setTextCursor(cursor);
                    } else {
                        QMessageBox::information(&window, "Find", "Text not found.");
                    }
                }
            }
        });
        QObject::connect(replaceNextButton, &QPushButton::clicked, [&]() {
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (currentEditor) {
                QString searchText = replaceFindLineEdit->text();
                QString replaceText = replaceWithLineEdit->text();
                if (!searchText.isEmpty()) {
                    QTextDocument *doc = currentEditor->document();
                    QTextCursor cursor = doc->find(searchText, currentEditor->textCursor());
                    if (!cursor.isNull()) {
                        cursor.insertText(replaceText);
                    } else {
                        QMessageBox::information(&window, "Replace", "Text not found.");
                    }
                }
            }
        });
        QObject::connect(replaceAllButton, &QPushButton::clicked, [&]() {
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
            if (currentEditor) {
                QString searchText = replaceFindLineEdit->text();
                QString replaceText = replaceWithLineEdit->text();
                if (!searchText.isEmpty()) {
                    QTextDocument *doc = currentEditor->document();
                    QTextCursor cursor = doc->find(searchText);
                    while (!cursor.isNull()) {
                        cursor.insertText(replaceText);
                        cursor = doc->find(searchText, cursor);
                    }
                }
            }
        });
        find_replace->show();
        return find_replace;
    };

    QObject::connect(&findButton, &QPushButton::clicked, [&]() {
        createFindReplcaeDialog(window, textEditStack);
    });
    QObject::connect(&replaceButton, &QPushButton::clicked, [&]() {
        createFindReplcaeDialog(window, textEditStack);
    });
    QObject::connect(&settingsButton, &QPushButton::clicked, [&]()
    {
        settings(window, app);
    });
    
    textEditStack->addWidget(firstEditor);
    if (firstEditor) {
        QObject::connect(firstEditor, &QTextEdit::textChanged, [fileTabBar, &tabModified]() {
            int index = 0;
            if (!tabModified[index]) {
                tabModified[index] = true;
                QString tabText = fileTabBar->tabText(index);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(index, tabText);
            }
        });
    }

    QObject::connect(upper_newAction, &QAction::triggered, [fileTabBar, textEditStack, &tabToFilePath, &tabModified, connectEditorSignals]() {
        LineNumberTextEdit *newEditor = new LineNumberTextEdit();
        newEditor->setPlaceholderText("Start writing!");
        textEditStack->addWidget(newEditor);
        int newTab = fileTabBar->addTab("New File 🔵");
        tabToFilePath[newTab] = "";
        tabModified[newTab] = false;
        fileTabBar->setCurrentIndex(newTab);
        textEditStack->setCurrentIndex(newTab);
        QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
            if (!tabModified[newTab]) {
                tabModified[newTab] = true;
                QString tabText = fileTabBar->tabText(newTab);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(newTab, tabText);
            }
        });
        connectEditorSignals(newEditor);
    });
    QObject::connect(upper_openAction, &QAction::triggered, &loadButton, &QPushButton::click);
    QObject::connect(upper_saveAction, &QAction::triggered, &saveButton, &QPushButton::click);
    QObject::connect(upper_saveAsAction, &QAction::triggered, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, file_type]() {
        int currentIndex = fileTabBar->currentIndex();
        if (currentIndex < 0) return;        
        LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->currentWidget());
        if (!currentEditor) return;
        QString suggestedName;
        QString tabText = fileTabBar->tabText(currentIndex);
        tabText.replace("🔵", "🔴");
        fileTabBar->setTabText(currentIndex, tabText);
        QString filePath = tabToFilePath[currentIndex];
        if (filePath.isEmpty()) {
            if (tabText == "New File") {
                QString currentType = file_type->text();
                QString defaultExt = "";
                if (fileTypeFilterMap.contains(currentType)) {
                    defaultExt = fileTypeFilterMap[currentType].second;
                } else {
                    defaultExt = ".txt";
                }
                suggestedName = "Untitled" + defaultExt;
            } else {
                suggestedName = fileTabBar->tabText(currentIndex);
            }
            filePath = QFileDialog::getSaveFileName(&window, "Save as", "C:/" + tabText, fileTypeFilterMap.value(file_type->text(), qMakePair(QString("All Files (*)"), QString(""))).first);
            if (filePath.isEmpty()) return;
        }        
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QString content = currentEditor->toPlainText();
            QTextStream out(&file);
            out << content;
            file.close();
            tabToFilePath[currentIndex] = filePath;
            tabModified[currentIndex] = false;
            QFileInfo fileInfo(filePath);
            QString baseName = fileInfo.fileName();
            fileTabBar->setTabText(currentIndex, baseName + " 🔵");
            window.setWindowTitle(filePath + " - Notepad+");
        } else {
            QMessageBox::warning(&window, "Error", "Could not write to file.");
        }        
    });

    QObject::connect(upper_saveAllAction, &QAction::triggered, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified]() {
        for (int i = 0; i < fileTabBar->count(); i++) {
            LineNumberTextEdit *currentEditor = static_cast<LineNumberTextEdit*>(textEditStack->widget(i));
            if (!currentEditor) continue;            
            QString filePath = tabToFilePath[i];
            if (filePath.isEmpty()) {
                filePath = QFileDialog::getSaveFileName(&window, "Save as", "C:/", "All Files (*)");
                if (filePath.isEmpty()) continue;
            }
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QString content = currentEditor->toPlainText();
                QTextStream out(&file);
                out << content;
                file.close();
                tabToFilePath[i] = filePath;
                tabModified[i] = false;
                QFileInfo fileInfo(filePath);
                QString baseName = fileInfo.fileName();
                fileTabBar->setTabText(i, baseName + " 🔵");
                window.setWindowTitle(filePath + " - Notepad+");
            } else {
                QMessageBox::warning(&window, "Error", "Could not write to file.");
            }
        }
    });

    QObject::connect(upper_exitAction, &QAction::triggered, &window, &QMainWindow::close);
    QObject::connect(upper_settingsAction, &QAction::triggered, [&window, &app]() {
        settings(window, app);
    });
    QObject::connect(upper_helpAction, &QAction::triggered, [&window]() {
        QMessageBox::information(&window, "Help", "Help will be added later.\nSorry for now.");
    });
    QObject::connect(upper_closeAction, &QAction::triggered, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, &saveButton]() {
        int currentIndex = fileTabBar->currentIndex();
        if (currentIndex >= 0) {
            if (tabModified[currentIndex]) {
                QMessageBox msgBox(&window);
                msgBox.setText("The document has been modified.");
                msgBox.setInformativeText("Do you want to save your changes?");
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();                
                switch (ret) {
                    case QMessageBox::Save:
                        saveButton.click();
                        return;
                    case QMessageBox::Cancel:
                        return;
                    default:
                        break;
                }
            }
            QWidget *widget = textEditStack->widget(currentIndex);
            textEditStack->removeWidget(widget);
            delete widget;            
            fileTabBar->removeTab(currentIndex);
            for (int i = currentIndex; i < fileTabBar->count(); i++) {
                tabToFilePath[i] = tabToFilePath[i+1];
                tabModified[i] = tabModified[i+1];
            }
            tabToFilePath.remove(fileTabBar->count());
            tabModified.remove(fileTabBar->count());
            if (fileTabBar->count() == 0) {
                LineNumberTextEdit *newEditor = new LineNumberTextEdit();
                newEditor->setPlaceholderText("Start writing!");
                textEditStack->addWidget(newEditor);
                int newTab = fileTabBar->addTab("New File 🔵");
                tabToFilePath[newTab] = "";
                tabModified[newTab] = false;
                QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
                    if (!tabModified[newTab]) {
                        tabModified[newTab] = true;
                        QString tabText = fileTabBar->tabText(newTab);
                        tabText.replace("🔵", "🔴");
                        fileTabBar->setTabText(newTab, tabText);
                    }
                });
            }
        }
    });
    QObject::connect(upper_closeAllAction, &QAction::triggered, [&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, &saveButton]() {
        bool anyModified = false;
        for (int i = 0; i < fileTabBar->count(); i++) {
            if (tabModified[i]) {
                anyModified = true;
                break;
            }
        }
        if (anyModified) {
            QMessageBox msgBox(&window);
            msgBox.setText("Some documents have been modified.");
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            
            switch (ret) {
                case QMessageBox::Save:
                    for(int i = 0 ; i < fileTabBar->count(); i++)
                    {
                        saveButton.click();
                    }
                    return;
                case QMessageBox::Cancel:
                    return;
                default:
                    break;
            }
        }
        while (fileTabBar->count() > 0) {
            QWidget *widget = textEditStack->widget(0);
            textEditStack->removeWidget(widget);
            delete widget;
            fileTabBar->removeTab(0);
        }
        tabToFilePath.clear();
        tabModified.clear();
        LineNumberTextEdit *newEditor = new LineNumberTextEdit();
        newEditor->setPlaceholderText("Start writing!");
        textEditStack->addWidget(newEditor);
        int newTab = fileTabBar->addTab("New File 🔵");
        tabToFilePath[newTab] = "";
        tabModified[newTab] = false;
        QObject::connect(newEditor, &QTextEdit::textChanged, [fileTabBar, newTab, &tabModified]() {
            if (!tabModified[newTab]) {
                tabModified[newTab] = true;
                QString tabText = fileTabBar->tabText(newTab);
                tabText.replace("🔵", "🔴");
                fileTabBar->setTabText(newTab, tabText);
            }
        });
    });

    restoreSession(window, fileTabBar, textEditStack, tabToFilePath, tabModified, file_type, char_line_label, connectEditorSignals);

    class WindowEventFilter : public QObject {
    public:
        WindowEventFilter(QMainWindow* window, FileTabBar* tabBar, QStackedWidget* stack, 
                         QMap<int, QString>* filePaths, QMap<int, bool>* modified, QPushButton* saveBtn)
            : QObject(window), mainWindow(window), fileTabBar(tabBar), 
              textEditStack(stack), tabToFilePath(filePaths), tabModified(modified), saveButton(saveBtn) {}

    protected:
        bool eventFilter(QObject *watched, QEvent *event) override {
            if (watched == mainWindow && event->type() == QEvent::Close) {
                saveSession(*tabToFilePath, fileTabBar, fileTabBar->currentIndex());
                bool anyModified = false;
                for (int i = 0; i < fileTabBar->count(); i++) {
                    if ((*tabModified)[i]) {
                        anyModified = true;
                        break;
                    }
                }
                if (anyModified) {
                    QMessageBox msgBox(mainWindow);
                    msgBox.setText("Some documents have been modified.");
                    msgBox.setInformativeText("Do you want to save your changes before closing?");
                    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Save);
                    int ret = msgBox.exec();
                    
                    switch (ret) {
                        case QMessageBox::Save:
                            for (int i = 0; i < fileTabBar->count(); i++) {
                                if ((*tabModified)[i]) {
                                    fileTabBar->setCurrentIndex(i);
                                    textEditStack->setCurrentIndex(i);
                                    saveButton->click();
                                }
                            }
                            break;
                        case QMessageBox::Cancel:
                            static_cast<QCloseEvent*>(event)->ignore();
                            return true;
                        default:
                            break;
                    }
                }
            }
            return QObject::eventFilter(watched, event);
        }

    private:
        QMainWindow* mainWindow;
        FileTabBar* fileTabBar;
        QStackedWidget* textEditStack;
        QMap<int, QString>* tabToFilePath;
        QMap<int, bool>* tabModified;
        QPushButton* saveButton;
    };

    window.installEventFilter(new WindowEventFilter(&window, fileTabBar, textEditStack, &tabToFilePath, &tabModified, &saveButton));

    mainlayout.addWidget(fileTabBar);
    mainlayout.addWidget(textEditStack);

    QWidget centralWidget;
    centralWidget.setLayout(&mainlayout);
    window.setCentralWidget(&centralWidget);
    window.setMinimumSize(800, 600);
    window.show();
    return app.exec();
}

////2345 linii inainte de curatenie
////1967 linii dupa curatenie
