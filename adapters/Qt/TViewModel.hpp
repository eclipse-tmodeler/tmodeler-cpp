/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QByteArray>
#include <QVariant>
#include <QString>
#include <map>
#include <string>
#include <model/TModel.h>

#include <QAbstractListModel>
#include <QVariant>
#include <QHash>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QModelIndex>
#include <string>


template<typename T>
class TItem;


template<typename T>
class TViewModel : public QAbstractListModel {

public:
    explicit TViewModel(QObject* parent = nullptr)
        : QAbstractListModel(parent)
    {
        m_objects = all();

        auto fieldsKeys = tms.fieldsKeys();

        int role = Qt::UserRole + 1;
        for (const auto& key : fieldsKeys) {
            m_roles[role] = key.c_str();
            m_roleNames[key] = role;
            role++;
        }

        observers();
    }

    virtual Tlist<T> all(){
        return tms.all();
    }

    virtual void observers(){
        tms.onCreate([this](auto keys) {
            QMetaObject::invokeMethod(this, [this, keys]() {
                for (int key : keys) {
                    int index = count();
                    if (index >= 0) {
                        beginInsertRows(QModelIndex(), index, index);
                        m_objects.add(tms.get(key));
                        endInsertRows();
                    }
                }
            }, Qt::QueuedConnection);
        });


        tms.onDelete([this](auto keys) {
            QMetaObject::invokeMethod(this, [this, keys]() {
                for(auto key : keys){
                    int index = m_objects.indexOf(key);
                    if (index >= 0) {
                        beginRemoveRows(QModelIndex(), index, index);
                        m_objects.remove(index);
                        endRemoveRows();
                    }
                }
            }, Qt::QueuedConnection);
        });

        tms.onUpdate([this](auto keys) {
            QMetaObject::invokeMethod(this, [this, keys]() {
                for(auto key : keys){
                    int index = m_objects.indexOf(key);
                    if (index >= 0) {
                        m_objects.get(index).parse(tms.get(key).data());
                        emit_dataChanged(this->index(index, 0), this->index(index, 0), roleNames().keys());
                        refresh();
                    }
                }
            }, Qt::QueuedConnection);
        });
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return static_cast<int>(m_objects.size());
    }

    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid() || index.row() >= static_cast<int>(m_objects.size()))
            return QVariant();

        auto obj = m_objects[index.row()];

        // Trouver le nom du champ à partir du rôle
        auto it = m_roles.find(role);
        if (it == m_roles.end())
            return QVariant();

        const QString& roleName = it.value();

        // Obtenir les données sous forme de JSON
        std::string json = obj.data();

        // Parser le JSON avec QJsonDocument
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
            return QVariant();

        QJsonObject objJson = doc.object();

        // Extraire la valeur correspondant au champ demandé
        QJsonValue val = objJson.value(roleName);
        return val.isUndefined() ? QVariant() : val.toVariant();
    }

    QHash<int, QByteArray> roleNames() const override {
        return m_roles;
    }

    Q_INVOKABLE void refresh() {
        beginResetModel();
        m_objects = all();
        endResetModel();
    }




    // CRUD
    // ====================== //
    // Fonctions utilitaires CRUD
    // ====================== //
    Q_INVOKABLE QVariantMap data(int index) {
        QVariantMap result;

        if (index < 0 || index >= m_objects.size())
            return result;

        T& obj = m_objects[index];
        std::string json = obj.data();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);

        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
            return result;

        QJsonObject jsonObj = doc.object();

        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            result[it.key()] = it.value().toVariant();
        }

        return result;
    }

    template<typename Ti>
    Q_INVOKABLE QObject* get(int index) {
        if (index < 0 || index >= m_objects.size())
            return nullptr;

        return new Ti(index, this);
    }

    Q_INVOKABLE QObject* get(int index) {
        if (index < 0 || index >= m_objects.size())
            return nullptr;

        return new TItem<T>(index, this);
    }

    Q_INVOKABLE bool create(const QVariantMap& data) {
        T obj;

        QJsonObject jsonObj;
        for (auto it = data.begin(); it != data.end(); ++it)
            jsonObj[it.key()] = QJsonValue::fromVariant(it.value());

        QJsonDocument doc(jsonObj);
        obj.parse(doc.toJson(QJsonDocument::Compact).toStdString());

        if (!obj.create()) {
            emit_errorOccurred("Erreur lors de la création.");
            return false;
        }

        return true;
    }

    Q_INVOKABLE bool update(int index, const QVariantMap& data) {
        if (index < 0 || index >= m_objects.size()){
            emit_errorOccurred("Index invalide pour la mise à jour.");
            return false;
        }

        T& obj = m_objects[index];

        QJsonObject jsonObj;
        for (auto it = data.begin(); it != data.end(); ++it)
            jsonObj[it.key()] = QJsonValue::fromVariant(it.value());

        QJsonDocument doc(jsonObj);
        obj.parse(doc.toJson(QJsonDocument::Compact).toStdString());

        if (!obj.update()) {
            emit_errorOccurred("Erreur lors de la mise à jour.");
            return false;
        }

        return true;
    }

    Q_INVOKABLE bool remove(int index) {
        if (index < 0 || index >= m_objects.size()){
            emit_errorOccurred("Index invalide pour la suppression.");
            return false;
        }

        T& obj = m_objects[index];

        if (!obj.del()) {
            emit_errorOccurred("Erreur lors de la suppression.");
            return false;
        }

        return true;
    }

    Q_INVOKABLE int count() const {
        return m_objects.size();
    }




    // more
    // ====================== //
    // Fonctions utilitaires supplémentaires
    // ====================== //

    // Retourne true si la liste d'objets est vide
    Q_INVOKABLE bool isEmpty() const {
        return m_objects.empty();
    }

    // Donne l’index d’un objet par sa clé (_id)
    Q_INVOKABLE int indexOf(int key) const {
        return m_objects.indexOf(key);
    }

    // Retourne un champ particulier (par son nom) d’un objet à l’index donné
    Q_INVOKABLE QVariant field(int index, const QString& name) const {
        if (index < 0 || index >= m_objects.size())
            return QVariant();

        T obj = m_objects[index];
        std::string json = obj.data();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject())
            return QVariant();

        QJsonObject objJson = doc.object();
        QJsonValue val = objJson.value(name);
        return val.isUndefined() ? QVariant() : val.toVariant();
    }

    // Donne accès brut à la liste des objets
    const Tlist<T>& objects() const {
        return m_objects;
    }

    // Permet de filtrer les objets dans C++ (pas QML) avec une fonction lambda
    void filterInCpp(std::function<bool(const T&)> predicate) {
        beginResetModel();
        Tlist<T> filtered;
        for (const T& obj : m_objects) {
            if (predicate(obj)) {
                filtered.add(obj);
            }
        }
        m_objects = filtered;
        endResetModel();
    }

    // méthode de filtrage simple (clé == valeur)
    Q_INVOKABLE void filter(const QString& key, const QVariant& value) {
        beginResetModel();
        Tlist<T> filtered;
        for (int i = 0; i < m_objects.size(); ++i) {
            auto obj = m_objects[i];
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(obj.data()));
            if (doc.isObject()) {
                QJsonObject json = doc.object();
                if (json.contains(key) && json.value(key).toVariant() == value) {
                    filtered.add(obj);
                }
            }
        }
        m_objects = filtered;
        endResetModel();
        emit_countChanged();
    }

    // méthode de chargement partiel (pagination)
    Q_INVOKABLE void loadMore(int offset, int limit) {
        beginResetModel();
        Tlist<T> list = all();
        Tlist<T> partial;
        for (int i = offset; i < offset + limit && i < list.size(); ++i) {
            partial.add(list[i]);
        }
        m_objects = partial;
        endResetModel();
        emit_countChanged();
    }

signals:
    void errorOccurred(const QString& message);
    void countChanged();

protected:
    virtual void emit_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) {}
    virtual void emit_errorOccurred(const QString& message) {}
    virtual void emit_countChanged() {}

protected:
    Tms<T> tms;
    Tlist<T> m_objects;
    QHash<int, QByteArray> m_roles;
    std::map<std::string, int> m_roleNames;
};


