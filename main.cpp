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
#include <QScreen>
#include <QGuiApplication>
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

///asta doar asa de tupeu daca vrei sa faci cand porneste prima oara sa tina minte ce are nevoie utilizatorul 
///daca are nevoie de un text editor sau de ide (adica sa ai tree de fisiere in stanga , sa ai un terminal jos sa implementezi pt python si pt c++ suport)

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
int radius = 6;

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
    ///vezi ce faci cu scrollbarul pe high contrast black
    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-size: 14px;
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

void eof_save(QTextEdit &textEdit) {
    QString content = textEdit.toPlainText();
    if (ostype == 1)
    {
        content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    } else 
    {
        content.replace("\r\n", "\n").replace("\r", "\n");
    }
}
/// nu o sa ma angajeze nimeni vreodata daca programez totul intro singur fisier da nah am vrut si eu sa ma distrez o data
Theme currentTheme;
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
    ///theme_layout->addStretch();
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

// Global extension map (declare near the top of your file, after includes)
QMap<QString, QString> extensionMap;
QMap<QString, QPair<QString, QString>> fileTypeFilterMap;

// Initialize the maps (call this before main or at the start of main)
void initializeFileMaps() {
    // Map of extensions to file types
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
    
    // Map file types to filter strings and default extensions
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
    
    // Return the file type if found, otherwise return "Normal text file"
    return extensionMap.value(extension, "Normal text file");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize file type maps
    initializeFileMaps();
    
    QMainWindow window;    
    window.setWindowIcon(QIcon(":/icon.png"));
    window.setWindowTitle("Basic ahh Text Editor");

    QString currentFilePath = "";
    QTextEdit textEdit;
    textEdit.setPlaceholderText("Hai la o cafea");
    QPushButton saveButton("Save");
    QPushButton loadButton("Load");
    QPushButton settingsButton("Settings");

    QPalette darkPalette;
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    window.setAutoFillBackground(true);
    app.setPalette(darkPalette);

    QMenuBar menuBar;
    QMenu fileMenu("File", &menuBar);
    QAction newAction("New", &menuBar);
    QAction openAction("Open", &menuBar);
    QAction saveAction("Save", &menuBar);

    QVBoxLayout mainlayout;
    QHBoxLayout buttonlayout;
    
    currentTheme = applySystemTheme(app);
    ///adauga iconita la butoane mrog asta optional , dar adaug o coloana cu nr randului in stanga
    ///adauga in dreapta jos ca la notepad linia si caracterul la care te afli
    ///adauga optiuna/functionalitate de a da quick save gen daca am deschis un fisier cu load atunci cand dau save daca vede ca 
    ///exista deja fisieru sau nu stiu cum testezu dar sa nu mai intre in explorer ca sa faca fisier nou de fiecare data
    ///daca incearca sa inchida aplicatia sa intrebe daca vrea sa salveze modificarile
    /// fa la save cand se deschide gui ala de save sa ai ca la notepad++ append extension si sa iti arate la numele fisierului
    /// extensia acestuia
    QStatusBar *statusBar = new QStatusBar(&window);
    window.setStatusBar(statusBar);
    
    // Create labels for line, column, etc.
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

    QLabel *char_line_label = new QLabel("Ln: 0  Col: 1");
    QLabel *positionLabel = new QLabel("Ln: 1  Col: 1 Pos: 1");
    QLabel *encodingLabel = new QLabel("UTF-8");
    QPushButton *eolButton = new QPushButton("Windows (CRLF)");
    eolButton->setFlat(true);
    eolButton->setCursor(Qt::PointingHandCursor);
    QMenu *eolMenu = new QMenu(eolButton);
    eolMenu->addAction("Windows (CRLF)");
    eolMenu->addAction("Unix (LF)");
    eolMenu->addAction("Mac (CR)");
    QFontMetrics fontMetrics(eolButton->font());
    int width = fontMetrics.horizontalAdvance("Windows (CRLF)") + 20; // Add some padding
    eolButton->setFixedWidth(width);
    QPushButton *cursor_caret = new QPushButton("INS");
    cursor_caret->setFlat(true);
    cursor_caret->setCursor(Qt::PointingHandCursor); /// adauga chestia asta cu mana si la restul butoanelor din program
    if(ostype == 1) {
        eolButton->setText("Windows (CRLF)");
        eolButton->setToolTip("Windows (CRLF)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    }
    else if(ostype == 2) {
        eolButton->setText("Mac (CR)");
        eolButton->setToolTip("Mac (CR)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    }
    else if(ostype == 3) {
        eolButton->setText("Unix (LF)");
        eolButton->setToolTip("Unix (LF)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    }
    
    // Add labels to status bar
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
    
    // Connect text cursor position changed signal to update the position label
    QObject::connect(&textEdit, &QTextEdit::cursorPositionChanged, [positionLabel, &textEdit]() {
        QTextCursor cursor = textEdit.textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        int position = cursor.position() + 1;
        positionLabel->setText(QString("Ln: %1  Col: %2 Pos: %3").arg(line).arg(column).arg(position));
    });
    QObject::connect(cursor_caret, &QPushButton::clicked, [cursor_caret, &textEdit]() {
    // pt un motiv anume nu suporta cursor caretul pentru overwrite deci si overwriteul o sa il aiba pe ala de la insert
        bool newMode = !textEdit.overwriteMode();
        textEdit.setOverwriteMode(newMode);
        cursor_caret->setText(newMode ? "OVR" : "INS");
    });
    QObject::connect(eolButton, &QPushButton::clicked, [eolButton, eolMenu]() {
        eolMenu->exec(eolButton->mapToGlobal(QPoint(0, -eolMenu->sizeHint().height())));
    });
    QObject::connect(eolMenu->actions().at(0), &QAction::triggered, [eolButton, &textEdit]() {
        eolButton->setText("Windows (CRLF)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    });
    QObject::connect(eolMenu->actions().at(1), &QAction::triggered, [eolButton, &textEdit]() {
        eolButton->setText("Unix (LF)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\r", "\n");
    });
    QObject::connect(eolMenu->actions().at(2), &QAction::triggered, [eolButton, &textEdit]() {
        eolButton->setText("Mac (CR)");
        QString content = textEdit.toPlainText();
        content.replace("\r\n", "\n").replace("\n", "\r");
    });

    QObject::connect(&saveButton, &QPushButton::clicked, [&]() {
        QString currentType = file_type->text();
        QString filter = "All Files (*)";
        QString defaultExt = "";
        
        // Get filter and default extension for current file type
        if (fileTypeFilterMap.contains(currentType)) {
            filter = fileTypeFilterMap[currentType].first;
            defaultExt = fileTypeFilterMap[currentType].second;
        }
        
        QString fileName = QFileDialog::getSaveFileName(&window, "Save as", "D:/", filter);

        if (!fileName.isEmpty()) {
            // Add default extension if none provided and we have a default
            if (!defaultExt.isEmpty() && !fileName.contains(".")) {
                fileName += defaultExt;
            }
            
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QString content = textEdit.toPlainText();
                
                // Convert to appropriate line endings before saving
                QString eolFormat = eolButton->text();
                if (eolFormat == "Windows (CRLF)") {
                    content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
                } else if (eolFormat == "Mac (CR)") {
                    content.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r");
                } else { // Unix (LF)
                    content.replace("\r\n", "\n").replace("\r", "\n");
                }
                
                QTextStream out(&file);
                out << content;
                file.close();
                currentFilePath = fileName;
                if (ostype == 1) {
                    currentFilePath.replace("/", "\\");
                }
                window.setWindowTitle(currentFilePath + " - Basic ahh Text Editor");
            } else {
                QMessageBox::warning(&window, "Error", "Could not write to file.");
            }
        }
    });

    QObject::connect(&loadButton, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(&window, "Open File", "C:/", "All Files (*);;C++ files (*.cpp);;Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray rawData = file.readAll();
                auto [lineCount, characterCount] = text_lines_characters(rawData);
                QString content = QString::fromUtf8(rawData);
                file.close();
                textEdit.setPlainText(content);
                currentFilePath = fileName;
                
                // Update window title
                if (currentFilePath.isEmpty()) {
                    window.setWindowTitle("Basic ahh Text Editor");
                } else {
                    if (ostype == 1) {
                        currentFilePath.replace("/", "\\");
                    }
                    window.setWindowTitle(currentFilePath + " - Basic ahh Text Editor");
                }
                
                // Update line count label
                char_line_label->setText(QString("length: %1  lines: %2").arg(characterCount).arg(lineCount));
                
                // Update file type button based on file extension
                file_type->setText(detectFileType(fileName));
            } else {
                QMessageBox::warning(&window, "Error", "Could not open file for reading.");
            }
        }
    });

    settingsButton.setToolTip("Settings");
    settingsButton.setText("⚙️");
    ///fa settings fara titlebar si cu fixed size
    ///cand se schimba stylesheetu sa se schimbe si culoarea de la titlebar
    ///adauga optiunea de rounded sau nu la toata aplicatia
    ///ok deci nu imi place cum arata dropdown cand e selectat e foarte naspa blue ala sa modifici in stylesheet
    /// adauga dropdown intrun horizontal layout ca sa fie in dreptul labelui
    /// adauga niste text lineuri in caz ca vrea custom
    /// a adauga 2 radio buttonuri in stanga la label si in stanga labelui custom pt zona custom ca sa stie cand da apply ce setare foloseste
    /// toate border-radius o sa aiba valoarea lor modificata cu text edit
    /// fa sa se mareasca interfata cand apar text lineurile
    /// adauga un buton apply la final si de reset si de close
    ///adauga o modalitate sa salveze ultimele fisiere pe care le ai avut deschise sa le deschida instant si setarile pe care le ales(ex sia facut tema custom)
    QObject::connect(&settingsButton, &QPushButton::clicked, [&]()
    {
        settings(window, app);
    });

    buttonlayout.addWidget(&saveButton);
    buttonlayout.addWidget(&loadButton);
    buttonlayout.addWidget(&settingsButton);

    mainlayout.addLayout(&buttonlayout);
    mainlayout.addWidget(&textEdit);

    QWidget centralWidget;
    centralWidget.setLayout(&mainlayout);
    window.setCentralWidget(&centralWidget);
    ///window.showMaximized();
    window.setMinimumSize(800, 600);
    window.show();
    return app.exec();
}
