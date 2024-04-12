#!/usr/bin/env python3
from distutils.dir_util import copy_tree
from distutils.file_util import copy_file
from subprocess import run, check_output
from multiprocessing import cpu_count

class Kernel:
	def __init__(self, platform_path, testcase_path, page_size_inst, page_size_data):
		self.platform_path = platform_path
		self.testcase_path = testcase_path

		self.page_size_inst = page_size_inst
		self.page_size_data = page_size_data

	def copy(self):
		copy_tree("{}/MAestro".format(self.platform_path),          "{}/MAestro".format(self.testcase_path), 	      update=1)
		copy_file("{}/MAestro/Makefile".format(self.platform_path), "{}/MAestro/Makefile".format(self.testcase_path), update=1)

	def build(self):
		NCPU = cpu_count()
		make = run(["make", "-C", "{}/MAestro".format(self.testcase_path), "-j", str(NCPU)])
		if make.returncode != 0:
			raise Exception("Error building kernel.")

	def check_size(self):
		path = "{}/MAestro/kernel.elf".format(self.testcase_path)

		out = check_output(["riscv64-elf-size", path, "-G"]).split(b'\n')[1].split(b'\t')[0].split(b' ')
		out = list(filter(lambda c: c != b'', out))

		isize = int(out[0])
		dsize = int(out[1]) + int(out[2])
					
		print("\n*************************** Kernel page size report ***************************")
		if isize <= self.page_size_inst and isize <= self.page_size_inst:
			print("Kernel instruction memory usage {}/{} bytes".format(str(isize).rjust(34), str(self.page_size_inst).ljust(6)))
			print("Kernel        data memory usage {}/{} bytes".format(str(dsize).rjust(34), str(self.page_size_data).ljust(6)))
		else:
			if isize > self.page_size_inst:
				raise Exception("Kernel instruction memory usage of {} is bigger than page size of {}".format(isize, self.page_size_inst))
			else:
				raise Exception("Kernel data memory usage of {} is bigger than page size of {}".format(dsize, self.page_size_data))
		print("************************* End kernel page size report *************************")
