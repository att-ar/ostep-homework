import subprocess
from optparse import OptionParser

filepath = "./process-run.py"
answer_args = ["-c", "-p"]


def problem(num: int, args: list, my_answer: str, show_answer: bool):
    print(f"--- Problem {num}:", *args)
    if not show_answer:
        subprocess.run([filepath, *args])
    else:
        print("My answer:", my_answer, "Answer:", sep="\n")
        subprocess.run([filepath, *args, *answer_args])


def main(do_problem: list[str], show_answer: bool = False):
    p = 1
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "5:100,5:100"]
        my_answer = """CPU should be in use 100% of the time since both processes have 5 CPU-bound instructions.
P0 will RUN and P1 will be READY.
Then P0 will be DONE and P1 will RUN."""
        problem(p, args, my_answer, show_answer)

    p = 2
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "4:100,1:0"]
        my_answer = """
        It will take 4 + 5 instruction units to finish.
        P0 will RUN 4 CPU bound instructions.
        Then P1 will launch it's 5-length IO instruction before finishing.
        This isn't optimal because P0 can run all 4 instructions in the 5-length wait.
        I was incorrect because I didn't take into account 2 CPU instructions
        - launching the IO
        - finishing the IO
        """
        problem(p, args, my_answer, show_answer)

    p = 3
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "1:0,4:100"]
        my_answer = """
        Switching the order of P2 means everything can finish in 7 instructions
        The 4 P1 instructions occur during the 5 blocked units of P0.
        """
        problem(p, args, my_answer, show_answer)

    p = 4
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "1:0,4:100", "-S", "SWITCH_ON_END"]
        my_answer = """
        This will be the exact same as Problem 2 but the opposite order.
        Since the switch on end flag means you can't take advantage of IO blocking to run another process.
        """
        problem(p, args, my_answer, show_answer)
    
    p = 5
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "1:0,4:100", "-S", "SWITCH_ON_IO"]
        my_answer = """
        This will be the exact same as Problem 3.
        """
        problem(p, args, my_answer, show_answer)

    p = 6
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "3:0,5:100,5:100,5:100", "-S", "SWITCH_ON_IO", "-I", "IO_RUN_LATER"]
        my_answer = """
        If smaller PID = higher priority then P0 should trigger 1 of its 3 IO instructions before P1, P2, P3.
        However, if the scheduling is round-robin then there will be a lot of wasted time because the first IO
        will trigger but the other 2 will happen at the very end of problem. This would not be effective resource use.
        """
        problem(p, args, my_answer, show_answer)
    
    p = 7
    if "all" in do_problem or str(p) in do_problem:
        args = ["-l", "3:0,5:100,5:100,5:100", "-S", "SWITCH_ON_IO", "-I", "IO_RUN_IMMEDIATE"]
        my_answer = """
        This will be more effective than problem 6 because P0 can run each of its 3 IO instructions before P1, P2, P3.
        So the IO blocking will pass the CPU to the other processes.
        """
        problem(p, args, my_answer, show_answer)

    p = 8
    if "all" in do_problem or str(p) in do_problem:
        list_tuples = [
            (
                ["-s", "1", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_LATER", "-S", "SWITCH_ON_IO", "-L", "2"],
                ["-s", "1", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_IMMEDIATE", "-S", "SWITCH_ON_IO", "-L", "2"],
            ),
            (
                ["-s", "2", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_LATER", "-S", "SWITCH_ON_IO", "-L", "2"],
                ["-s", "2", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_IMMEDIATE", "-S", "SWITCH_ON_IO", "-L", "2"],
            ),
            (
                ["-s", "3", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_LATER", "-S", "SWITCH_ON_IO", "-L", "2"],
                ["-s", "3", "-l", "3:50,3:50,3:50,3:50,3:50", "-I", "IO_RUN_IMMEDIATE", "-S", "SWITCH_ON_IO", "-L", "2"],
            ),
        ]
        my_answer = """
        When there is a chance of multiple IO instructions from one process
        it makes more sense to switch_on_io and io_run_immediate in order to maximize
        the time a blocked process is in the background while a cpu-bound process can run.

        Notes after running:
        - When there are enough processes with io instructions, io later vs io immediate doesn't make a big difference.
          This is especially the case when the io blocking lasts the same amount for all io instructions.
        - Profiling really is important lol.
        """
        for i, tuple_args in enumerate(list_tuples, start = 1):
            print(f"--- Seed = {i}\n")
            for args in tuple_args:
                problem(p, args, my_answer, show_answer)
            print()


if __name__ == "__main__":
    parser = OptionParser()

    def parse_problem_list(_option, _opt_str, value, parser):
        parser.values.problem = [item.strip() for item in value.split(",")]
    parser.add_option(
        "-p",
        "--problem",
        default="all",
        help="problem number(s) or all (comma separated, e.g. 1,2,3)",
        action="callback",
        callback=parse_problem_list,
        type="string",
        dest="problem",
    )
    parser.add_option(
        "-c",
        help="compute answers for me",
        action="store_true",
        default=False,
        dest="solve",
    )
    (options, _args) = parser.parse_args()

    main(options.problem, options.solve)
