#!/usr/bin/python3

import os
import gzip
import shutil
import yaml
import hashlib
import zipfile
from lxml import etree


curr_path = os.getcwd()
files_path = curr_path + os.sep + "files"

#print(softwares)

with open("manifest_pattern.cig.xml", "r") as f:
    root = etree.XML(f.read())
    products = root[0][0]
    with open("cig-product.xml", "r") as f:
        vendor_product = etree.XML(f.read())
        for product in vendor_product.findall("./"):
            products.append(product)

etree.indent(root)
et = etree.ElementTree(root)
et.write("manifest.xml", pretty_print=True)

