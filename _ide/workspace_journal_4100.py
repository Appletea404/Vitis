# 2026-04-07T09:47:09.139817
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.create_platform_component(name = "platform_FND",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_FND/design_1_FND_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

platform = client.get_component(name="platform_FND")
status = platform.build()

comp = client.create_app_component(name="hello_world_FND",platform = "$COMPONENT_LOCATION/../platform_FND/export/platform_FND/platform_FND.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

status = platform.build()

comp = client.get_component(name="hello_world_FND")
comp.build()

vitis.dispose()

