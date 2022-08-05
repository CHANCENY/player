#include "stylesloader.h"

StylesLoader::StylesLoader()
{

}

QString StylesLoader::stylesloader()
{
    QSqlQuery query;
    QString stylespath;

    query.prepare("SELECT * FROM styles WHERE Status = 'True';");
    if(query.exec())
    {
        while(query.next())
        {
            stylespath = query.value(1).toString();
        }
    }

    if(stylespath != nullptr)
    {
        QString styles;

        QFile myfile(stylespath);
        myfile.open(QIODevice::ReadOnly);
        if(myfile.isOpen())
        {
            styles = myfile.readAll();
            myfile.close();
        }
        return styles;
    }
    return nullptr;
}
