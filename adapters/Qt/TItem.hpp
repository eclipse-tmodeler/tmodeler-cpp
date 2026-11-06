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

#include <QObject>
#include <QVariantMap>
#include "TViewModel.hpp"


template<typename T>
class TItem : public QObject {

public:
    explicit TItem(QObject* parent = nullptr)
        : QObject(parent), m_index(-1), m_viewModel(nullptr) {}

    TItem(int index, TViewModel<T>* viewModel, QObject* parent = nullptr)
        : QObject(parent), m_index(index), m_viewModel(viewModel) {}

    QVariantMap get() const { return m_viewModel->data(m_index); }

    void set(const QVariantMap& data) { m_viewModel->update(m_index, data); }

protected:
    int m_index;
    TViewModel<T>* m_viewModel;
};
