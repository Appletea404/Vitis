# 2026-04-08T09:35:14.328241
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.create_platform_component(name = "platform_TIMER",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_TIMER/design_1_TIMER_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

comp = client.create_app_component(name="hello_world_TIMER",platform = "$COMPONENT_LOCATION/../platform_TIMER/export/platform_TIMER/platform_TIMER.xpfm",domain = "standalone_microblaze_riscv_0",template = "hello_world")

platform = client.get_component(name="platform_TIMER")
status = platform.build()

status = platform.build()

comp = client.get_component(name="hello_world_TIMER")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

