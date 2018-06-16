import os
import string
import itertools


def genEntity(componentTypes, entityTypeId): 
    codelines = []
    entityName = "Entity{0}".format(entityTypeId)
    entityTypeId += 1

    # struct Entity0
    codelines.append("struct {0} : public Entity".format(entityName))

    # {
    codelines.append("{")

    # CComponentOne m_CComponentOne;
    # ...
    # CComponentN m_CComponentN;
    for component in componentTypes:
        codelines.append("\t{0} m_{0}".format(component))

    # }
    codelines.append("}")

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
    codelines = genEntity(componentNames, entityTypeId)
    
    for code in codelines:
        print(code)
        
main()