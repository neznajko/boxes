#!/usr/bin/env python3
import argparse

def get_dest(box): ############################### get destination index
    return box.index(' ')

def get_orig(box): # >>>>>>>>>>>>>>>>>>>>>>>>>>> get originating indexes
    dest = get_dest(box)
    orig = list(range(0, dest - 1)) + list(range(dest + 2, boxez - 1))
    orig.append(-1) # Sentinel
    return orig

def get_box_str(box): # ================================================
    return "|" + "|".join(box) + "|"

def get_orig_str(orig): # ++++++++++++++++++++++++++++++++++++++++++++++
    orig = orig[:-1] # ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; avoid the Sentinel
    return "orig: " + ", ".join(map(str, orig))

def svap(orig, dest, box): # :::::::::::::::::::::::::::::::::::::::::::
    t = list(box)
    y = slice(orig, orig + 2)
    z = slice(dest, dest + 2)
    t[y], t[z] = t[z], t[y]
    return ''.join(t)

class Poz: # %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Position Class
    def __init__(self, box): ###__########################## constructor
        self.box = box ###############;;;;;;################# box string
        self.dest = get_dest(box) ###`````` index of the first empty box
        self.orig = get_orig(box) #####--#### list of all possible moves

    def __str__(self): #################### called by the print function
        box_str = get_box_str(self.box)
        orig_str = get_orig_str(self.orig)
        return box_str + f"\ndest = {self.dest}\n" + orig_str

    def move(self, orig): # * * * * * * * * * * * * * * * * * * * * * *
        self.__init__(svap(orig, self.dest, self.box))

def playing_mode(box): # # # # # # # # # # # # # # # # # # # # # # # # # 
    poz = Poz(box)
    while True:
        print(poz)
        try:
            orig = int(input("Enter orig: "))
        except: # hit Enter for example to exit
            return
        if orig in poz.orig[:-1]: # >>>>>>>>>>> dodge the Sentinel again
            print("OK")
            poz.move(orig)
        else:
            print("Naah")

def ckmate(box): # >LL>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> check mate
    return box.index('b') > box.rindex('a')

                 # >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> stack
                        # { { { { { { { { { { { { {{{{{{{{{{{{{{{{ Consa
# ......................................................................
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ backslash
# ``````````````````````````````````````````````````````````````````````
log = [] # | | | | | | | | | | | History stack with box string positions
#  +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +   +
def nextnode(node):
    """ We find next node by executing order 66 """
    poz = node.poz
    dest = poz.dest
    box = poz.box
    
    while True:
        orig = poz.orig[node.i]
        node.i = node.i + 1
        if orig == -1:
            break
        box = svap(orig, dest, box)
        if (box not in log):
            return Node(box)
        box = svap(dest, orig, box)

preorder = None #               the solution with lowest level from srch
def srch(root): #  ` ' , " `   * ' ' " * `   " ' ' * * ` preorder search
    box = root.poz.box
    log.append(box)
######## ck for mate positions * * * * * * * * * * * * * * * * * * * * *
    global preorder
    if ckmate(box):
        if preorder is None or root < preorder:
            preorder = root
######## thatz the loop ________________________________________________
    while True:
        q = nextnode(root)
        if q == None:
            break
######## link and set node tree level //////////////////////////////////
        q.parent = root
        q.level = root.level + 1
######## here we go again %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        srch(q)

def reveal(node, msg): #  _ * . - * . ` * . , * . $ * . . * . Top Secret
    #`'"!?:;-,.,-;:?!"'`'"!?:;-,.,-;:?!"'`'"!? log stack should be empty
    log.clear() # OKAY
    while True:
        log.append(node.poz.box) # ! ' ' ' ' ' ' | ' ' ' ' ' ' push back
        node = node.parent
        if not node: # \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ ve ar don!?
            break

    print(msg)
    log.reverse()
    print(*map(get_box_str, log), sep = '\n')

class Node: # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Node Class
    def __init__(self, box): #------------------------------ constructor
        self.poz = Poz(box) #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ position
        self.i = 0 #============================= poz.orig index counter
        self.level = 0 #++++++++++++++++++++++++++++++++ node tree level
        self.parent = None #|||||||||||||||||||||||||||||||| parent node

    def __str__(self): # ///////////////////////////////////////////////
        return self.poz.__str__() + \
               f"\ni = {self.i}\nlevel = {self.level}" + \
               f"\nparent = {self.parent.__repr__()}"

    def __lt__(self, other): # =============================== less than
        return self.level < other.level

def planb(box): # ------------------------------------------------------
    root = Node(box)
    srch(root)
    if preorder is None: #__ _  _   _    _     _      _      no solution
        print("._o")
    else:
        reveal(preorder, "There is no Plan A:")

def depth_srch(root):
    """ some usefull information """
    parent = [root]
    children = []

    log.clear()
    log.append(root.poz.box)
    
    while True:
        for n in parent:
            if ckmate(n.poz.box):
                return n
            while True:
                j = nextnode(n)
                if j is None:
                    break
                j.parent = n
                j.level = n.level + 1
                children.append(j)
                log.append(j.poz.box)
                
        if not children:
            return None
        parent.clear()
        parent, children = children, parent

def planc(box):
    root = Node(box)
    j = depth_srch(root)
    if j is None:
        print("._0")
    else:
        reveal(j, "Mate in " + str(j.level))
    
parser = argparse.ArgumentParser()
parser.add_argument("-p", action = "store_true", help = "playing mode")
parser.add_argument("box", nargs = '?', help = '"box string"')
args = parser.parse_args()
# ----------------------------------------------------------------------
pmode = args.p ####################################### playing mode flag

if args.box: #                 ck if box string is given on command line
    box = args.box 
else: 
    box = input("Please enter a box string: ")

boxez = len(box) ####################################### number of boxes

if pmode:
    playing_mode(box)
else:
    planb(box)
    planc(box)
################################################################### log:
