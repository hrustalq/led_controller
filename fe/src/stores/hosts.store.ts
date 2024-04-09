import { defineStore } from "pinia";
import { Host } from "../entities/host.entity";

import { socket } from "../socket";

import { UpdateActiveHostsPayload } from "../dto/update-active-hosts.dto";
import { onMounted, ref } from "vue";
import getAllHosts from "../api/hosts/get-all-hosts";


export const useHostsStore = defineStore("hosts", () => {
  const hosts = ref<Host[]>([]);

  const fetchAllHosts = async () => {
    await getAllHosts().then((response) => {
      hosts.value = [...response];
    });
  }

  onMounted(() => {
    socket.on("updateActiveHosts", (payload: UpdateActiveHostsPayload[]) => {
      hosts.value.forEach((host, idx) => {
        if (hosts.value[idx] === payload[idx]) {
          host.active = true;
        };
      })
    });
    socket.on("updateAllHosts", (payload: Host[]) => {
      if (payload) {
        hosts.value = [...payload];
      }
    });
    fetchAllHosts();
  });

  return {
    hosts,
  }
});
