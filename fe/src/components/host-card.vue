<template>
  <VCard class="mx-auto" max-width="300" :color="color" :subtitle="subtitleValue" :title="host.name">
    <template v-slot:prepend>
      <VIcon icon="mdi-alarm"></VIcon>
    </template>
    <template v-slot:append>
      <VAvatar size="24">
      </VAvatar>
    </template>
  </VCard>
</template>

<script setup lang="ts">
import { VCard, VAvatar, VIcon } from "vuetify/components";

import { Host } from "../entities/host.entity";
import { computed } from "vue";

const props = defineProps<{ host: Host }>();

const color = computed(() =>
  props.host.active ? 'disabled' : 'secondary'
);
const subtitleValue = computed(() => {
  return props.host.active ? "Занят" : "Свободен";
});
</script>

<style scoped>

</style>
