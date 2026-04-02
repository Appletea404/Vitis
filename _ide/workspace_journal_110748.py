# 2026-04-03T02:17:36.267320
import vitis

client = vitis.create_client()
client.set_workspace(path="Vitis")

platform = client.create_platform_component(name = "LED",hw_design = "$COMPONENT_LOCATION/../../SoC/project_1_LED/design_1_LED_wrapper.xsa",os = "standalone",cpu = "microblaze_riscv_0",domain_name = "standalone_microblaze_riscv_0")

client.delete_component(name="LED")

vitis.dispose()

