#include <QApplication>
#include <QFile>
#include <QFileView>
#include <QTextDocument>
#include <qt5diff/qtdiff.h>

int main(int argc, char** argv)
{
    // Create a new application
    QApplication app(argc, argv);

    // Load the diff document
    QFile file("diff_example.txt");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        printf("Error opening file: %s\n", file.errorString().c_str());
        return 1;
    }

    // Create a new QTextDocument object
    QTextDocument doc;

    // Create a new QTextEdit object to display the diff
    QTextEdit edit(&doc);

    // Read the contents of the file into the QTextEdit object
    edit.setText(file.readAll());

    // Create a new QTextEdit object to display the diff
    QTextEdit diff(&doc);

    // Parse the diff
    qt5diff::diff(&diff);

    // Show the diff
    diff.show();

    // Show the contents of the file
    file.close();
    printf("Contents of diff_example.txt:\n");
    printf("%s\n", file.readAll().toUtf8().data());

    // Wait for user input
    QWaitInstance* wait = new QWaitInstance();
    wait->waitUntil(QWaitInstance::Idle);
    return app.exec();
}