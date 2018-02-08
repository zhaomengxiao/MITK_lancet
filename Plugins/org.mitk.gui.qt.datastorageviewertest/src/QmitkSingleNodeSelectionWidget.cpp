/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#include "QmitkSingleNodeSelectionWidget.h"

#include <QmitkNodeSelectionDialog.h>

QmitkSingleNodeSelectionWidget::QmitkSingleNodeSelectionWidget(QWidget* parent) : QWidget(parent), m_InvalidInfo("Error. Select data."), m_IsOptional(false), m_SelectOnlyVisibleNodes(false)
{
  m_Controls.setupUi(this);

  m_Controls.btnLock->setChecked(false);
  this->UpdateInfo();

  m_Controls.nodeInfo->installEventFilter(this);

  connect(m_Controls.btnLock, &QPushButton::clicked, this, &QmitkSingleNodeSelectionWidget::OnLock);
}

void QmitkSingleNodeSelectionWidget::SetDataStorage(mitk::DataStorage* dataStorage)
{
  if (m_DataStorage != dataStorage)
  {
    m_DataStorage = dataStorage;
  }
};

void QmitkSingleNodeSelectionWidget::SetNodePredicate(mitk::NodePredicateBase* nodePredicate)
{
  if (m_NodePredicate != nodePredicate)
  {
    m_NodePredicate = nodePredicate;
  }
};

mitk::NodePredicateBase* QmitkSingleNodeSelectionWidget::GetNodePredicate() const
{
  return m_NodePredicate;
}

mitk::DataNode::Pointer QmitkSingleNodeSelectionWidget::GetSelectedNode() const
{
  return m_SelectedNode;
};

QString QmitkSingleNodeSelectionWidget::GetInvalidInfo() const
{
  return m_InvalidInfo;
};

bool QmitkSingleNodeSelectionWidget::GetSelectionIsOptional() const
{
  return m_IsOptional;
};

void QmitkSingleNodeSelectionWidget::OnLock(bool locked)
{
};

bool QmitkSingleNodeSelectionWidget::eventFilter(QObject *obj, QEvent *ev)
{
  if (obj == m_Controls.nodeInfo)
  {
    if (ev->type() == QEvent::MouseButtonRelease)
    {
      this->EditSelection();
    }

    return true;
  }

  return false;
}

void QmitkSingleNodeSelectionWidget::EditSelection()
{
  QmitkNodeSelectionDialog* dialog = new QmitkNodeSelectionDialog(this);

  dialog->SetDataStorage(m_DataStorage.Lock());
  dialog->SetNodePredicate(m_NodePredicate);
  NodeList list;
  list.append(m_SelectedNode);
  dialog->SetCurrentSelection(list);
  dialog->SetSelectOnlyVisibleNodes(m_SelectOnlyVisibleNodes);

  if (dialog->exec())
  {
    auto nodes = dialog->GetSelectedNodes();
    if (nodes.empty())
    {
      m_SelectedNode = nullptr;
    }
    else
    {
      m_SelectedNode = nodes.first();
    }

    this->UpdateInfo();
    emit CurrentSelectionChanged(nodes);
  }
};

void QmitkSingleNodeSelectionWidget::UpdateInfo()
{
  if (m_SelectedNode.IsNull())
  {
    if (m_IsOptional)
    {
      m_Controls.nodeInfo->setText("Empty");
    }
    else
    {
      m_Controls.nodeInfo->setText(m_InvalidInfo);
    }
  }
  else
  {
    m_Controls.nodeInfo->setText(QString::fromStdString(m_SelectedNode->GetName()));
  }

};

void QmitkSingleNodeSelectionWidget::SetSelectOnlyVisibleNodes(bool selectOnlyVisibleNodes)
{
  m_SelectOnlyVisibleNodes = selectOnlyVisibleNodes;
};

void QmitkSingleNodeSelectionWidget::SetCurrentSelection(NodeList selectedNodes)
{
  if (!m_Controls.btnLock->isChecked())
  {
    mitk::DataNode::Pointer newSelected = nullptr;
    NodeList list;
    bool changed = false;

    if (!selectedNodes.empty() && m_SelectedNode != selectedNodes.first())
    {
      newSelected = selectedNodes.first();
      list.append(m_SelectedNode);
      changed = true;
    }
    else if(selectedNodes.empty() && m_SelectedNode != nullptr)
    {
      newSelected = nullptr;
      changed = true;
    }

    if (changed)
    {
      emit CurrentSelectionChanged(list);
      m_SelectedNode = newSelected;
      this->UpdateInfo();
    }

  }
};

void QmitkSingleNodeSelectionWidget::SetSelectionLock(bool locked)
{
  m_Controls.btnLock->setChecked(locked);
};

void QmitkSingleNodeSelectionWidget::SetSelectionUnlock(bool unlocked)
{
  m_Controls.btnLock->setChecked(!unlocked);
};

void QmitkSingleNodeSelectionWidget::SetInvalidInfo(QString info)
{
  m_InvalidInfo = info;
};

void QmitkSingleNodeSelectionWidget::SetSelectionIsOptional(bool isOptional)
{
  m_IsOptional = isOptional;
};
