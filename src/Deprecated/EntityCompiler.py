import os
import string
import itertools
import datetime

# Should define a "compiler" class that keeps track of scope, expressions etc... 
def genEntity(componentTypes, entityTypeId): 
    codelines = []
    entityName = "GeneratedEntity{0}".format(entityTypeId)

    # struct Entity0 : public Entity
    codelines.append("struct {0} : public Entity\n".format(entityName))
    codelines.append("{\n")

    # CComponentN m_CComponentN;
    for component in componentTypes:
        codelines.append("\t{0} m_{0}\n".format(component))

    codelines.append("\n") 
    codelines.append("\tvirtual uint64_t getComponentMask() const override\n")
    codelines.append("\t{\n")
  
    # return 0 | CComponentOne::s_type ... | CComponentN::s_type;  
    maskExpression = "\t\treturn 0"
    for component in componentTypes:
        maskExpression += (" | {0}::s_type".format(component))

    maskExpression += ";\n"
    codelines.append(maskExpression)
    codelines.append("\t}\n")
    codelines.append("\n") 

    codelines.append("\tvirtual void* getComponent(uint64_t type) override\n")
    codelines.append("\t{\n")
    codelines.append("\t\tswitch (type)\n")
    codelines.append("\t\t{\n")

    # TODO generate some sort of lookup table indexed with type here instead of switch
    for component in componentTypes:
        codelines.append("\t\tcase {0}::s_type:\n".format(component))
        codelines.append("\t\t{\n")
        codelines.append("\t\t\treturn &m_{0};\n".format(component))
        codelines.append("\t\t}\n")
			
    codelines.append("\t}\n")

    codelines.append("};\n\n")

    return codelines

def main():

    componentDirectoy = "C:/Users/Philipp/Documents/work/Phoenix/src/ECS/Components"

    if not os.path.exists(componentDirectoy) :
        print("Error: Failed to locate component directory: ", componentDirectoy)
        return -1

    componentNames = []

    for file in os.listdir(componentDirectoy):
        componentNames.append(file.replace(".hpp", ""))     

    entityTypeId = 0
    generatedEntities = []

    # generate powerset of componentTypes
    for numComponents in range(1, len(componentNames) + 1):
        permutations = itertools.combinations(componentNames, numComponents)
        for subset in permutations:
            generatedEntities.append(genEntity(subset , entityTypeId))
            entityTypeId += 1

    entityFilePath = "C:/Users/Philipp/Documents/work/Phoenix/src/ECS/Entities.hpp"

    file = open(entityFilePath,'w')
    
    file.write("#pragma once\n")
    file.write("#include <ECS/Entity.hpp>\n\n")
    file.write("// Entities.hpp: Generated {0} UTC\n".format(datetime.datetime.utcnow()))
    file.write("// A set of all possible entities generated from the existing set of components.\n\n")

    for component in componentNames:
        file.write("#include <ECS/Components/{0}.hpp>\n".format(component))

    file.write("\n")

    for entityCode in generatedEntities:
        file.writelines(entityCode)

main()