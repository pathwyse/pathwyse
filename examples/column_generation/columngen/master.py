import gurobipy as grb

class RMP:
    def __init__(self, K, num_nodes):
        self.num_nodes = num_nodes
        self.K = K
        self.model = grb.Model("master")
        self.model.Params.LogToConsole = 0
        #self.model.Params.presolve = 0
        self.pool = list()
        return
    
    def buildModel(self, maxObj):
        model = self.model
        
        dummy_cost = maxObj
        dummy = model.addVar(lb=0.0, name="dummy", vtype=grb.GRB.CONTINUOUS)

        #Obj: minimize cost
        obj = grb.LinExpr()
        obj.add(dummy, dummy_cost) 
        model.setObjective(obj, grb.GRB.MINIMIZE)

        #Con: every node is visited at least once
        for i in range (1, self.num_nodes - 1):
            model.addLConstr(dummy >= 1, name = "cVisited[" + str(i) + "]")

        #Con: At most K vehicles are used
        model.addLConstr(dummy <= self.K, name = "cVehicles")
        
    def addColumn(self, cost, col):
        model = self.model
        self.pool.append(col)

        #Create new variable
        z = model.addVar(lb = 0.0, name = "z" + str(len(self.pool)), vtype = grb.GRB.CONTINUOUS)

        #Update objective function
        obj = model.getObjective()
        obj.add(z, cost)
        model.setObjective(obj, grb.GRB.MINIMIZE)

        #Update node constraints
        for i in range (1, len(col) - 1):
            node = col[i]
            con = model.getConstrByName("cVisited[" + str(node) + "]")
            model.chgCoeff(con, z, 1)

        #Update vehicle constraint  
        con = model.getConstrByName("cVehicles")
        model.chgCoeff(con, z, 1)

        model.update()
    
    def solve(self):
        self.model.optimize()

    def getDuals(self):
        model = self.model

        #get Mu Duals (node prize)
        mu = [0.0] * self.num_nodes
        for i in range(1, self.num_nodes - 1):
            con = model.getConstrByName("cVisited[" + str(i) +"]")
            mu[i] = con.getAttr("Pi")

        #get gamma dual
        con = model.getConstrByName("cVehicles")
        gamma = con.getAttr("Pi")

        return [mu, gamma]

    def getObj(self):
        return self.model.ObjVal

    def writeModel(self):
        self.model.write('master.mps')