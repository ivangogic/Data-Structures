import java.util.Stack;

public class AVLTree<T extends Comparable<T>> {

    private Node<T> root;
    private int size;

    public boolean isEmpty() {
        return root == null;
    }

    public int size() {
        return size;
    }

    public void clear() {
        root = null;
        size = 0;
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

        Node<T> current = root;
        Stack<Node<T>> stack = new Stack<>();
        while (current != null) {
            stack.push(current);
            if (current.value.compareTo(value) == 0)
                return;
            else if (current.value.compareTo(value) > 0)
                current = current.left;
            else
                current = current.right;
        }

        Node<T> newNode = new Node<>(value);
        while (stack.size() > 0) {
            current = stack.pop();
            if (current.value.compareTo(newNode.value) < 0)
                current.right = newNode;
            else
                current.left = newNode;
            updateHeight(current);
            current = rebalance(current);
            newNode = current;
        }
        root = newNode;

        size++;
    }

    public void remove(T value) {
        if (value == null)
            return;

        Node<T> current = root;
        Stack<Node<T>> stack = new Stack<>();
        while (current != null && !current.value.equals(value)) {
            stack.push(current);
            if (current.value.compareTo(value) > 0)
                current = current.left;
            else
                current = current.right;
        }

        if (current == null)
            return;

        Node<T> newNode = null;
        if (current.left == null || current.right == null) {
            if (current.left == null && current.right == null) {
                Node<T> parent = stack.peek();
                if (parent.left != null && parent.left.value.compareTo(value) == 0)
                    parent.left = null;
                else
                    parent.right = null;
            }
            else if (current.left == null) {
                current = current.right;
                newNode = current;
            } else {
                current = current.left;
                newNode = current;
            }
        } else {
            stack.push(current);
            Node<T> next = inOrderNext(current, stack);
            current.value = next.value;
            Node<T> parent = stack.peek();
            if (parent.left != null && parent.left.value.compareTo(next.value) == 0)
                parent.left = (getHeight(next) == 0) ? null : next.right;
            else
                parent.right = (getHeight(next) == 0) ? null : next.right;
            if (next.right != null)
                newNode = next.right;
        }
        while (stack.size() > 0) {
            current = stack.pop();
            if (newNode != null) {
                if (current.value.compareTo(newNode.value) < 0)
                    current.right = newNode;
                else
                    current.left = newNode;
            }
            updateHeight(current);
            current = rebalance(current);
            newNode = current;
        }
        root = newNode;

        size--;
    }

    private Node<T> inOrderNext(Node<T> node, Stack<AVLTree.Node<T>> stack) {
        Node<T> next = node.right;
        while (next.left != null) {
            stack.push(next);
            next = next.left;
        }

        return next;
    }

    private Node<T> rotateLeft(Node<T> node) {
        Node<T> rChild = node.right;

        node.right = rChild.left;
        rChild.left = node;

        updateHeight(node);
        updateHeight(rChild);

        return rChild;
    }

    private Node<T> rotateRight(Node<T> node) {
        Node<T> lChild = node.left;

        node.left = lChild.right;
        lChild.right = node;

        updateHeight(node);
        updateHeight(lChild);

        return lChild;
    }

    private Node<T> rebalance(Node<T> node) {
        if (BF(node) < -1) {
            if (BF(node.left) > 0)
                node.left = rotateLeft(node.left);
            node = rotateRight(node);
        }
        else if (BF(node) > 1) {
            if (BF(node.right) < 0)
                node.right = rotateRight(node.right);
            node = rotateLeft(node);
        }

        return node;
    }

    private static class Node<G extends Comparable<G>> {
        G value;
        Node<G> left, right;
        int height;

        private Node(G value) {
            this.value = value;
        }
    }

    private int getHeight(Node<T> node) {
        return node == null ? -1 : node.height;
    }

    private void updateHeight(Node<T> node) {
        int lHeight = getHeight(node.left), rHeight = getHeight(node.right);
        node.height = Math.max(lHeight, rHeight) + 1;
    }

    private int BF(Node<T> node) {
        return getHeight(node.right) - getHeight(node.left);
    }
}
