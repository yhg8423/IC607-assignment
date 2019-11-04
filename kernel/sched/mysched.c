/*
 * Lottery scheduling class for IC 607 assignement
*/
#include "sched.h"
#include <linux/list.h>

// initializae run queue for each core
void init_mysched_rq(struct mysched_rq *mysched_rq)
{
    INIT_LIST_HEAD(&mysched_rq->queue); // initialize queue in mysched run queue (mysched_rq)
    mysched_rq->nr_running = 0; // set nr_running = 0 in mysched run queue (mysched_rq)
    mysched_rq->max_ticket = 0; // set max_ticket = 0 in mysched run queue (mysched_rq)
    printk(KERN_INFO "***[MYSCHED] Mysched class is online \n"); // print success initialization
}

static void update_curr_mysched(struct rq *rq)
{
    // could be empty
}

// put the task into the run queue
static void enqueue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    INIT_LIST_HEAD(&p->mysched.run_list); // initialize run list in mysched entity in task struct p
    list_add_tail(&p->mysched.run_list, &rq->mysched.queue); // add run_list node in mysched entity in task struct before queue node in run queue
    rq->mysched.nr_running++; // increase nr_running because the task is put into the run queue
    p->mysched.ticket = 500 - p->prio * 500 / 140; // set tickek in mysched entity of each task with priority
    rq->mysched.max_ticket += p->mysched.ticket; // update max_ticket in run queue for remembering total ticket of tasks.
    printk(KERN_INFO"***[MYSCHED] Enqueue: nr_running = %d pid = %d\n", rq->mysched.nr_running, p->pid); // print success enqueue
}

// take out the task from the runq
static void dequeue_task_mysched(struct rq *rq, struct task_struct *p, int flags)
{
	if(!list_empty(&rq->mysched.queue)) { // if there is task in queue, take out the task from run queue
		list_del_init(&p->mysched.run_list); // delete node from run_list in mysched entity & reinitialize run_list
		rq->mysched.nr_running--; // decrease nr_running because the task is taken out from the run queue
        rq->mysched.max_ticket -= p->mysched.ticket; // update max_ticket in run queue because the task is taken out
		printk(KERN_INFO"***[MYSCHED] Decueue: nr_running = %d pid = %d\n", rq->mysched.nr_running, p->pid); // print success dequeue
	}
}

// checking priority for scheduling preemption
static void check_preempt_curr_mysched(struct rq *rq, struct task_struct *p, int flags)
{
    resched_curr(rq); // set scheduling request 
}

static void put_prev_task_mysched(struct rq *rq, struct task_struct *prev)
{
    //could be empty
}

// pick the next task( scheduling algorithm location )
static struct task_struct * 
pick_next_task_mysched(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    struct task_struct *next_p = NULL; // pointer to the next task which will be chosen
	struct mysched_rq *mysched_rq = &rq->mysched; // pointer to mysched run queue in run queue

    // return NULL & finish function when nr_running is zero
	if(!mysched_rq->nr_running) {
		return NULL;
    }

    printk(KERN_INFO "***[MYSCHED] pick next task cpu(%d)\n", cpu_of(rq)); // print number of cpu for scheduling

    unsigned int total_ticket = rq->mysched.max_ticket; // unsigned integer variable to store sum of tickets of tasks in run queue
    unsigned int lucky_ticket = 0; // declare unsigned integer variable which will store the lucky ticket(winner) in the algorithm
    int ticket_sum = 0; // declare integer variable which will store the sum of tickets of tasks which are selected during loop

    // selecting lucky ticket(winner) randomly
    while(lucky_ticket <= 0) {
        get_random_bytes(&lucky_ticket, sizeof(unsigned int));
		lucky_ticket = (lucky_ticket % total_ticket) + 1;
    }

    printk(KERN_INFO "***[MYSCHED] total = [%d]\n", total_ticket); // print total_ticket of the tasks
    printk(KERN_INFO "***[MYSCHED] winner = [%d]\n", lucky_ticket); // print lucky_ticket(winner) in the algorithm

    struct list_head *q; // pointer to list_head for pick the task
    // loop for select next task which will be scheduled
    list_for_each(q, &mysched_rq->queue) {
        next_p = list_entry(q, struct task_struct, mysched.run_list); // select next task with list entry() function
        ticket_sum += next_p->mysched.ticket; // update ticket_sum to compare with lucky ticket

        printk(KERN_INFO "***[MYSCHED] pid = [%d] ticket = [%d]\n", next_p->pid, next_p->mysched.ticket); // print info of selected task
        printk(KERN_INFO "***[MYSCHED] sum = [%d] \n", ticket_sum); // print ticket_sum

        // if ticket_sum is bigger than lucky_ticket, the task will be scheduled
        if(ticket_sum >= lucky_ticket) {
            break;
        }
    }

    printk(KERN_INFO "***[MYSCHED] [sum >= winner] Next task pid = [%d] \n", next_p->pid); // print task which will be scheduled next
	
    // return picked next task
	return next_p;
}

static int select_task_rq_mysched(struct task_struct *p, int cpu, int sd_flag, int flags)
{
    // could be empty
}

static void set_curr_task_mysched(struct rq *rq)
{
    // could be empty
}

static void task_tick_mysched(struct rq *rq, struct task_struct *p, int queued)
{
    // could be empty because it does not affect scheduling algorithm
}

static void prio_changed_mysched(struct rq *rq, struct task_struct *p, int oldprio)
{
    // could be empty
}

// put the task into mysched scheduler
static void switched_to_mysched(struct rq *rq, struct task_struct *p)
{
    // if sched_class of task is not equal to mysched_sched_class, it will put the task into mysched scheduler
    if (p->sched_class == &mysched_sched_class) {
        return;
    }
	enqueue_task_mysched(rq, p, 0);
}

const struct sched_class mysched_sched_class = {
    // pointer to next scheduling class in hierarchy
    .next = &fair_sched_class,

    // when task can be runnable
    .enqueue_task = enqueue_task_mysched,

    // when task needs to wait
    .dequeue_task = dequeue_task_mysched,

    // check if the task which can be runnable should prevent current running task
    .check_preempt_curr = check_preempt_curr_mysched,

    // pick the next task from run queue
    .pick_next_task = pick_next_task_mysched,
    .put_prev_task = put_prev_task_mysched,
#ifdef CONFIG_SMP
    .select_task_rq = select_task_rq_mysched,
    .set_cpus_allowed = set_cpus_allowed_common,
#endif
    // set other options
    .set_curr_task = set_curr_task_mysched,
    .prio_changed = prio_changed_mysched,
    .task_tick = task_tick_mysched,
    .switched_to = switched_to_mysched,
    .update_curr = update_curr_mysched
};





