#include "commentmodel.h"

CommentModel::CommentModel(QObject *parent)
    : QAbstractListModel(parent), loaded{false},
      currentrequestnumber{0}, finalrequestnumber{0}
{
//    connect(&networkrequest, &Network::complete, this, &CommentModel::parseCommentInfo);
//    networkrequest.get();
    QVariantList a;
    a << 1 << "SeedPuller" << "1 Day Ago" << "This is my text" << 0 << 0 << 1;
    QVariantList d;
    d << 2 << "SeedPuller" << "1 Day Ago" << "This is my text" << 0 << 0 << 0;
    QVariantList c;
    c << 3 << "SeedPuller" << "1 Day Ago" << "This is my text" << 0 << 0 << 0;
    QVariantList b;
    b << 4 << "SomeLongeUsername" << "1 Day Ago" << "This is my text" << 0 << 0 << 0;
    QVariantList f;
    f << 5 << "SeedPuller" << "1 Day Ago" << "This is my text reply" << 1 << 20 << 0;
    vlist.append(a);
    vlist.append(b);
    vlist.append(c);
    vlist.append(d);
//    vlist.append(f);
}


int CommentModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return vlist.size();
}

QVariant CommentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || vlist.size() <= 0 || role <= ROLE_START || role >= ROLE_END)
        return QVariant();

    QVariantList temp = vlist.at(index.row());
    const int column = role - (ROLE_START + 1);
    return temp[column];
}


//bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    int indexrow = index.row();
//    if (data(index, role) != value && vlist.size() > indexrow) {

//        switch (role) {

//            case fnameRole:
//                vlist[indexrow][1] = value.toString();
//            break;
//            case debtRole:
//                vlist[indexrow][2] = value.toString();
//            break;
//            case picRole:
//                vlist[indexrow][3] = value.toString();
//            break;
//        }
//        emit dataChanged(index, index, QVector<int>() << role);
//        return true;
//    }
//    return false;
//}

Qt::ItemFlags CommentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> CommentModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    int column_number{0};
    for (int i{ROLE_START + 1}; i != ROLE_END; ++i, ++column_number) {
        roles.insert(i, this->columns[column_number]);
    }
    return roles;
}

bool CommentModel::insert(const int id, const QString& author,
                          const QString& date, const QString& text,
                          const int cmparent)
{
    int rowcount = rowCount();
    beginInsertRows(QModelIndex(), rowcount, rowcount);

    QVariantList temp;
    temp.append(id);
    temp.append(author);
    temp.append(text);
    temp.append(date);
    temp.append(cmparent);
    vlist.push_back(temp);
    endInsertRows();
    return true;
}

bool CommentModel::insert(int position, int count, const QVariantList &data)
{
    return true;
}

//void CommentModel::parseCommentInfo(const QByteArray &datas)
//{
//    disconnect(&networkrequest, &Network::complete, this, &CommentModel::parseCommentInfo);
//    connect(&networkrequest, &Network::complete, this, &CommentModel::parsePostInfo);
//    QJsonDocument jsonresponse = QJsonDocument::fromJson(datas);
//    QJsonArray jsonobject = jsonresponse.array();
//    for (int i{0}; i < finalrequestnumber; ++i) {
//        getPostInfo(jsonobject[i].toInt());
//    }
//}

void CommentModel::parseCommentInfo(const QByteArray &data)
{
    QJsonDocument jsonresponse = QJsonDocument::fromJson(data);
    QJsonObject jsonobject = jsonresponse.object();
    QVariantList temp;
    int parent{0};
    const int id{jsonobject["id"].toInt()};
    QDateTime date;
    date.setSecsSinceEpoch(jsonobject["time"].toInt());

    if (jsonobject["parent"].toInt() != postid) {
        parent = jsonobject["parent"].toInt();
    }else {

    }

    if (jsonobject["kids"] != QJsonValue()) {
        QJsonArray kids = jsonobject["kids"].toArray();
        foreach (QJsonValue child, kids) {
            getCommentInfo(id);
        }
    }

//    temp.append(jsonobject["by"].toString());
//    temp.append(jsonobject["url"].toString());
//    temp.append(jsonobject["title"].toString());
//    temp.append(date.toString("dd MMM hh:mm"));
//    temp.append(comments);
//    temp.append(jsonobject["score"].toInt());
//    vlist.push_back(temp);
    insert(id, jsonobject["by"].toString(), date.toString("dd MMM hh:mm"),
            jsonobject["text"].toString(), parent);

    checkRequestJobDone();
}

void CommentModel::getCommentInfo(int id)
{
    ++finalrequestnumber;
    networkrequest.setUrl(QUrl(commentinfoapi.toString() + QString::number(id) + ".json"));
    networkrequest.get();
}

void CommentModel::checkRequestJobDone()
{
    if (currentrequestnumber == finalrequestnumber - 1) {
        finalrequestnumber = 0;
        currentrequestnumber = 0;
        setLoaded(true);
        return;
    }
    ++currentrequestnumber;
}

int CommentModel::getPostid() const
{
    return postid;
}

void CommentModel::setPostid(int value)
{
    postid = value;
}


//bool Model::remove(int index, const QModelIndex& parent) {
////        qDebug() << "removing index number: " << index;
//        beginRemoveRows(parent, index, index);
//        if (!removeRow(vlist[index][0].toInt())) {
//            qDebug() << db.getError();
//        }
//        vlist.removeAt(index);
//        endRemoveRows();
//        return true;
//}


bool CommentModel::getLoaded() const
{
    return loaded;
}

void CommentModel::setLoaded(bool value)
{
    if (loaded == value) {
        return;
    }
    loaded = value;
    emit loadedChanged(value);
}

void CommentModel::getCommentInfo()
{
//    qDebug() << "entered id to getPostInfo is: " << id;
//    qDebug() << "sending request to : " << postinfoapi.toString() + QString::number(id) + ".json";
    networkrequest.setUrl(QUrl(commentinfoapi.toString() + QString::number(postid) + ".json"));
    networkrequest.get();
}
