import os
import string
import itertools

# Should define a "compiler" class that keeps track of scope, expressions etc... 
def genEntity(componentTypes, entityTypeId): 
    codelines = []
    entityName = "Entity{0}".format(entityTypeId)

    # struct Entity0 : public Entity
    codelines.append("struct {0} : public Entity".format(entityName))
    codelines.append("{")

    # CComponentN m_CComponentN;
    for component in componentTypes:
        codelines.append("\t{0} m_{0}".format(component))

    codelines.append("") 
    codelines.append("\tvirtual uint64_t getComponentMask() const override")
    codelines.append("\t{")
  
    # return 0 | CComponentOne::s_type ... | CComponentN::s_type;  
    maskExpression = "\t\treturn 0"
    for component in componentTypes:
        maskExpression += (" | {0}::s_type".format(component))

    maskExpression += ";"
    codelines.append(maskExpression)
    codelines.append("\t}")
    codelines.append("") 

    codelines.append("\tvirtual void* getComponent(uint64_t type) override")
    codelines.append("\t{")
    codelines.append("\t\tswitch (type)")
    codelines.append("\t\t{")

    # TODO generate some sort of lookup table indexed with type here instead of switch
    for component in componentTypes:
        codelines.append("\t\tcase {0}::s_type:".format(component))
        codelines.append("\t\t{")
        codelines.append("\t\t\treturn &m_{0};".format(component))
        codelines.append("\t\t}")
			
    codelines.append("\t}")

    codelines.append("};")

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

    for entityCode in generatedEntities:
        for line in entityCode:
            print(line)

main()