public class RedBlackTree<T extends Comparable<T>> {

    private enum Color {
        BLACK,
        RED
    }
    private static class Node<G extends Comparable<G>> {
        G value;
        Node<G> left, right, parent;
        Color color;

        private Node(G value) {
            this.value = value;
        }

        private Node() {}
    }

    private class NilNode<G extends Comparable<G>> extends Node<G> {
        private NilNode() {
            super();
            this.color = Color.BLACK;
        }
    }

    private Node<T> root;
    private int size;

    public boolean isEmpty() {
        return root == null;
    }

    public int size() {
        return size;
    }

    public void clear() {
        size = 0;
        root = null;
    }

    public boolean contains(T value) {
        if (value == null)
            return false;

        Node<T> current = root;
        while (current != null) {
            if (current.value.compareTo(value) == 0)
                return true;
            else if (current.value.compareTo(value) > 0)
                current = current.left;
            else
                current = current.right;
        }
        return false;
    }

    public void add(T value) {
        if (value == null)
            return;

        Node<T> current = root, parent = null;
        while (current != null) {
            parent = current;
            if (current.value.compareTo(value) == 0)
                return;
            else if (current.value.compareTo(value) > 0)
                current = current.left;
            else
                current = current.right;
        }

        Node<T> newNode = new Node<>(value);
        newNode.color = Color.RED;
        newNode.parent = parent;
        if (parent == null)
            root = newNode;
        else if (parent.value.compareTo(value) < 0)
            parent.right = newNode;
        else
            parent.left = newNode;

        fixAdd(newNode);
        size++;
    }

    private void fixAdd(Node<T> node) {
        Node<T> parent = node.parent;

        if (parent == null || parent.color == Color.BLACK)
            return;

        Node<T> grandParent = parent.parent;

        if (grandParent == null) {
            parent.color = Color.BLACK;
            return;
        }

        Node<T> uncle = getUncle(node);

        if (uncle != null && uncle.color == Color.RED) {
            parent.color = Color.BLACK;
            uncle.color = Color.BLACK;
            grandParent.color = Color.RED;
            fixAdd(grandParent);
            return;
        }

        // uncle is black
        if (parent == grandParent.left) {
            if (node == parent.right) {
                rotateLeft(parent);
                parent = node;
            }
            rotateRight(grandParent);
        } else {
            if (node == parent.left) {
                rotateRight(parent);
                parent = node;
            }
            rotateLeft(grandParent);
        }
        parent.color = Color.BLACK;
        grandParent.color = Color.RED;
    }

    private Node<T> getUncle(Node<T> node) {
        Node<T> grandParent = node.parent.parent;
        if (grandParent.right == node.parent)
            return grandParent.left;
        else
            return grandParent.right;
    }

    public void remove(T value) {
        if (value == null)
            return;

        Node<T> current = root;
        while (current != null && !current.value.equals(value)) {
            if (current.value.compareTo(value) > 0)
                current = current.left;
            else
                current = current.right;
        }

        if (current == null)
            return;

        Node<T> movedUpNode;
        Color removedColor;
        if (current.left == null || current.right == null) {
            movedUpNode = remove01(current);
            removedColor = current.color;
        } else {
            Node<T> next = inOrderNext(current);
            current.value = next.value;
            movedUpNode = remove01(next);
            removedColor = next.color;
        }

        if (removedColor == Color.BLACK) {
            fixRemove(movedUpNode);
            if (movedUpNode.getClass() == NilNode.class)
                replaceParentsChild(movedUpNode.parent, movedUpNode, null);
        }
        size--;
    }

    private Node<T> remove01(Node<T> node) {
        if (node.left == null && node.right == null) {
            Node<T> newChild = node.color == Color.BLACK ? new NilNode<>() : null;
            replaceParentsChild(node.parent, node, newChild);
            return newChild;
        } else if (node.left != null) {
            replaceParentsChild(node.parent, node, node.left);
            return node.left;
        } else {
            replaceParentsChild(node.parent, node, node.right);
            return node.right;
        }
    }

    private Node<T> inOrderNext(Node<T> node) {
        Node<T> next = node.right;
        while (next.left != null)
            next = next.left;

        return next;
    }

    private void fixRemove(Node<T> node) {
        if (node == root)
            return;

        Node<T> parent = node.parent, sibling = getSibling(node);

        if (sibling.color == Color.RED) {
            parent.color = Color.RED;
            sibling.color = Color.BLACK;
            if (node == parent.left)
                rotateLeft(parent);
            else
                rotateRight(parent);
            sibling = getSibling(node);
        }

        // sibling is black
        if (isBlack(sibling.left) && isBlack(sibling.right)) {
            sibling.color = Color.RED;
            if (parent.color == Color.RED)
                parent.color = Color.BLACK;
            else
                fixRemove(parent);
        } else {
            if (node == parent.left && isBlack(sibling.right) || node == parent.right && isBlack(sibling.left)) {
                if (node == parent.left) {
                    sibling.left.color = Color.BLACK;
                    sibling.color = Color.RED;
                    rotateRight(sibling);
                } else {
                    sibling.right.color = Color.BLACK;
                    sibling.color = Color.RED;
                    rotateLeft(sibling);
                }
            }
            sibling = getSibling(node);
            sibling.color = parent.color;
            parent.color = Color.BLACK;
            if (node == parent.left) {
                sibling.right.color = Color.BLACK;
                rotateLeft(parent);
            } else {
                sibling.left.color = Color.BLACK;
                rotateRight(parent);
            }
        }
    }

    private Node<T> getSibling(Node<T> node) {
        return node == node.parent.left ? node.parent.right : node.parent.left;
    }

    private boolean isBlack(Node<T> node) {
        return node == null || node.color == Color.BLACK;
    }

    private void rotateRight(Node<T> node) {
        Node<T> parent = node.parent;
        Node<T> leftChild = node.left;

        node.parent = leftChild;
        node.left = leftChild.right;
        if (leftChild.right != null)
            leftChild.right.parent = node;
        leftChild.right = node;

        replaceParentsChild(parent, node, leftChild);
    }

    private void rotateLeft(Node<T> node) {
        Node<T> parent = node.parent;
        Node<T> rightChild = node.right;

        node.parent = rightChild;
        node.right = rightChild.left;
        if (rightChild.left != null)
            rightChild.left.parent = node;
        rightChild.left = node;

        replaceParentsChild(parent, node, rightChild);
    }

    private void replaceParentsChild(Node<T> parent, Node<T> oldChild, Node<T> newChild) {
        if (parent == null)
            root = newChild;
        else if (parent.left == oldChild)
            parent.left = newChild;
        else
            parent.right = newChild;

        if (newChild != null)
            newChild.parent = parent;
    }
}
