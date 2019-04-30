class Queue
{

    public static final int QUEUE_SIZE = 10;
    int[] stack;
    int back;

    public Queue()
    {
        back = -1;
        stack = new int[QUEUE_SIZE];
    }

    public int peek()
    {
        return stack[0];
    }

    public void push(int value)
    {
        stack[++back] = value;
        System.out.printf("%d PUSH !\n", stack[back]);
    }

    public int pop()
    {
        System.out.printf("%d POP !\n", stack[0]);
        for (int i = 1; i <= back; ++i)
        {
            stack[i - 1] = stack[i];
        }
        return stack[back--];
    }

    public void printQueue()
    {
        System.out.printf("\n-----QUEUE LIST-----\n");

        for (int i = 0; i <= back; ++i)
        {
            System.out.printf("%d\n", stack[i]);
        }

        System.out.printf("-----END OF LIST-----\n");
    }
}

public class q4
{

    public static void main(String[] args)
    {

        Queue q = new Queue();

        q.push(5);
        q.push(2);
        q.push(3);
        q.push(4);
        q.push(1);

        q.printQueue();

        q.pop();
        q.pop();
        q.push(15);

        System.out.printf("FRONT OF QUEUE : %d\n", q.peek());

        q.printQueue();

        q.pop();
        q.pop();
        q.pop();
        q.pop();

        q.push(30);

        q.printQueue();
    }
}
