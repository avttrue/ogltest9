#include "materiallibrary.h"
#include "material.h"

#include <QDebug>
#include <QDir>

bool MaterialLibrary::load(const QString &filename)
{
    QFile mtlfile(filename);

    if(! mtlfile.exists()) { qCritical() << "File not exist:" << filename; return false; }
    if(! mtlfile.open(QFile::ReadOnly))  { qCritical() << "File not opened:" << filename; return false; }

    for(auto o: m_Materials) delete o;
    m_Materials.clear();

    auto apath = QFileInfo(filename).absolutePath() + QDir::separator();
    Material* newMtl = nullptr;

    QTextStream input(&mtlfile);
    qDebug() << "Reading" << filename << "...";

    bool ok = true;
    while(!input.atEnd())
    {
        auto str = input.readLine(); if(str.isEmpty()) continue;
        auto strlist = str.split(' '); strlist.removeAll("");
        auto key = strlist.at(0);

        if (key == "#") { qDebug() << str; }
        else if(key == "newmtl")
        {
            add(newMtl);
            newMtl = new Material();
            if(strlist.size() > 1) newMtl->setName(strlist.at(1));
            else
            {
                newMtl->setName(QString::number(m_Materials.size()));
                qCritical() << "Error at line (material name not present):" << str;
            }
        }
        else if(newMtl && key.toLower() == "ns")
        {
            if(strlist.size() > 1) newMtl->setShines(strlist.at(1).toFloat(&ok));
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(newMtl && key.toLower() == "ka")
        {
            if(strlist.size() > 3)
            {
                newMtl->setAmbienceColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                   strlist.at(2).toFloat(&ok),
                                                   strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(newMtl && key.toLower() == "kd")
        {
            if(strlist.size() > 3)
            {
                newMtl->setDiffuseColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                  strlist.at(2).toFloat(&ok),
                                                  strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(newMtl && key.toLower() == "ks")
        {
            if(strlist.size() > 3)
            {
                newMtl->setSpecularColor(QVector3D(strlist.at(1).toFloat(&ok),
                                                   strlist.at(2).toFloat(&ok),
                                                   strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(newMtl && key.toLower() == "map_kd")
        {
            if(strlist.size() > 1)
            {
                auto file = apath + strlist.at(1);
                if(QFile(file).exists()) newMtl->setDiffuseMap(file);
                else { qCritical() << "File not exists:" << file; ok = false; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }

        //else continue;

        add(newMtl);
    }

    mtlfile.close();
    qDebug() << "... done" << filename;
    return ok;
}

void MaterialLibrary::add(Material *m)
{
    if(!m) return;
    for(auto o: m_Materials) if(o == m) return;
    m_Materials.append(m);
}

Material *MaterialLibrary::get(int index)
{
    if(index > -1 && index < m_Materials.size()) return m_Materials.at(index);
    return nullptr;
}

Material *MaterialLibrary::get(const QString &name)
{
    for(auto o: m_Materials) if(o->Name() == name) return o;
    return nullptr;
}

int MaterialLibrary::count()
{
    return m_Materials.size();
}
