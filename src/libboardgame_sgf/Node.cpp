//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Node.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Node.h"

#include "MissingProperty.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

Property::Property(const string& id, const vector<string>& values)
    : id(id),
      values(values)
{
    LIBBOARDGAME_ASSERT(! values.empty());
}

//-----------------------------------------------------------------------------

Node::Node()
    : m_parent(nullptr)
{
}

void Node::append(unique_ptr<Node> node)
{
    node->m_parent = this;
    if (! m_first_child)
        m_first_child = move(node);
    else
        get_last_child()->m_sibling = move(node);
}

Node& Node::create_new_child()
{
    unique_ptr<Node> node(new Node());
    node->m_parent = this;
    Node& result = *(node.get());
    auto last_child = get_last_child();
    if (last_child == nullptr)
        m_first_child = move(node);
    else
        last_child->m_sibling = move(node);
    return result;
}

void Node::delete_variations()
{
    if (m_first_child)
        m_first_child->m_sibling.reset(nullptr);
}

Property* Node::find_property(const string& id) const
{
    auto property = m_first_property.get();
    while (property != nullptr)
    {
        if (property->id == id)
            break;
        property = property->next.get();
    }
    return property;
}

const vector<string> Node::get_multi_property(const string& id) const
{
    auto property = find_property(id);
    if (property == nullptr)
        return vector<string>();
    else
        return property->values;
}

bool Node::has_property(const string& id) const
{
    return find_property(id) != nullptr;
}

const Node& Node::get_child(unsigned i) const
{
    LIBBOARDGAME_ASSERT(i < get_nu_children());
    auto child = m_first_child.get();
    while (i > 0)
    {
        child = child->m_sibling.get();
        --i;
    }
    return *child;
}

unsigned Node::get_child_index(const Node& child) const
{
    auto current = m_first_child.get();
    unsigned i = 0;
    while (true)
    {
        if (current == &child)
            return i;
        current = current->m_sibling.get();
        LIBBOARDGAME_ASSERT(current != nullptr);
        ++i;
    }
}

Node* Node::get_last_child() const
{
    auto node = m_first_child.get();
    if (node == nullptr)
        return nullptr;
    while (node->m_sibling)
        node = node->m_sibling.get();
    return node;
}

unsigned Node::get_nu_children() const
{
    unsigned n = 0;
    auto child = m_first_child.get();
    while (child != nullptr)
    {
        ++n;
        child = child->m_sibling.get();
    }
    return n;
}

const Node* Node::get_previous_sibling() const
{
    if (m_parent == nullptr)
        return nullptr;
    auto child = &m_parent->get_first_child();
    if (child == this)
        return nullptr;
    do
    {
        if (child->get_sibling() == this)
            return child;
        child = child->get_sibling();
    }
    while (child != nullptr);
    LIBBOARDGAME_ASSERT(false);
    return nullptr;
}

const string& Node::get_property(const string& id) const
{
    auto property = find_property(id);
    if (property == nullptr)
        throw MissingProperty(id);
    return property->values[0];
}

const string& Node::get_property(const string& id,
                                 const string& default_value) const
{
    auto property = find_property(id);
    if (property == nullptr)
        return default_value;
    else
        return property->values[0];
}

void Node::make_first_child()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current_child = m_parent->m_first_child.get();
    if (current_child == this)
        return;
    while (true)
    {
        auto sibling = current_child->m_sibling.get();
        if (sibling == this)
        {
            unique_ptr<Node> tmp = move(m_parent->m_first_child);
            m_parent->m_first_child = move(current_child->m_sibling);
            current_child->m_sibling = move(m_sibling);
            m_sibling = move(tmp);
            return;
        }
        current_child = sibling;
    }
}

bool Node::move_property_to_front(const string& id)
{
    auto current = m_first_property.get();
    Property* last = nullptr;
    while (true)
    {
        if (current == nullptr)
            return false;
        if (current->id == id)
        {
            if (last != nullptr)
            {
                unique_ptr<Property> tmp = move(last->next);
                last->next = move(current->next);
                current->next = move(m_first_property);
                m_first_property = move(tmp);
            }
            return true;
        }
        last = current;
        current = current->next.get();
    }
}

void Node::move_down()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current = m_parent->m_first_child.get();
    if (current == this)
    {
        unique_ptr<Node> tmp = move(m_parent->m_first_child);
        m_parent->m_first_child = move(m_sibling);
        m_sibling = move(m_parent->m_first_child->m_sibling);
        m_parent->m_first_child->m_sibling = move(tmp);
        return;
    }
    while (true)
    {
        auto sibling = current->m_sibling.get();
        if (sibling == this)
        {
            if (! m_sibling)
                return;
            unique_ptr<Node> tmp = move(current->m_sibling);
            current->m_sibling = move(m_sibling);
            m_sibling = move(current->m_sibling->m_sibling);
            current->m_sibling->m_sibling = move(tmp);
            return;
        }
        current = sibling;
    }
}

void Node::move_up()
{
    LIBBOARDGAME_ASSERT(has_parent());
    auto current = m_parent->m_first_child.get();
    if (current == this)
        return;
    Node* prev = nullptr;
    while (true)
    {
        auto sibling = current->m_sibling.get();
        if (sibling == this)
        {
            if (prev == nullptr)
            {
                make_first_child();
                return;
            }
            unique_ptr<Node> tmp = move(prev->m_sibling);
            prev->m_sibling = move(current->m_sibling);
            current->m_sibling = move(m_sibling);
            m_sibling = move(tmp);
            return;
        }
        prev = current;
        current = sibling;
    }
}

bool Node::remove_property(const string& id)
{
    auto property = m_first_property.get();
    Property* last = nullptr;
    while (property != nullptr)
    {
        if (property->id == id)
            break;
        last = property;
        property = property->next.get();
    }
    if (property == nullptr)
        return false;
    if (last != nullptr)
        last->next = move(property->next);
    else
        m_first_property = move(property->next);
    return true;
}

unique_ptr<Node> Node::remove_child(Node& child)
{
    auto node = &m_first_child;
    unique_ptr<Node>* previous = nullptr;
    while (true)
    {
        if (node->get() == &child)
        {
            unique_ptr<Node> result = move(*node);
            if (previous == nullptr)
                m_first_child = move(child.m_sibling);
            else
                (*previous)->m_sibling = move(child.m_sibling);
            result->m_parent = nullptr;
            return result;
        }
        previous = node;
        node = &(*node)->m_sibling;
        LIBBOARDGAME_ASSERT(node);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
